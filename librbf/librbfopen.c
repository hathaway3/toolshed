/********************************************************************
 * open.c - OS-9 open/create routines
 *
 * $Id$
 ********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#ifndef WIN32
#include <dirent.h>
#endif

#include "cocotypes.h"
#include "os9path.h"
#include "cococonv.h"
#include "cocosys.h"
#include "debug.h"
#include "util.h"


static int init_pd(os9_path_id * path, int mode);
static int term_pd(os9_path_id path);
static int init_bitmap(os9_path_id path);
static int term_bitmap(os9_path_id path);
static int init_lsn0(os9_path_id path);
static int term_lsn0(os9_path_id path);
static void _os9_truncate_seg_list(os9_path_id path);
error_code _os9_file_exists(os9_path_id folder_path, char *filename);
int validate_pathlist(os9_path_id * path, char *pathlist);


/*
 * _os9_create()
 *
 * Create a file
 */
error_code _os9_create(os9_path_id * path, char *pathlist, int mode,
		       int perms)
{
	error_code ec = 0;
	*path = NULL;


	/* 1. Allocate & initialize path descriptor. */

	ec = init_pd(path, mode);

	if (ec != 0)
	{
		return ec;
	}


	/* 2. Attempt to validate the pathlist. */

	ec = validate_pathlist(path, pathlist);

	if (ec != 0)
	{
		term_pd(*path);
		*path = NULL;

		return ec;
	}


	/* 3. Determine if disk is being open in raw mode. */

	if (pathlist[strlen(pathlist) - 1] == '@')
	{
		/* 1. Yes, raw mode. */

		(*path)->israw = 1;
	}
	else
	{
		/* 1. No, normal mode. */

		(*path)->israw = 0;
	}


	{
		fd_stats newFD;
		time_t now;
		int newLSN;
		char *filename = NULL;
		os9_path_id parent_path = NULL;
		os9_dir_entry newDEntry;

		term_pd(*path);
		*path = NULL;

	      aa:
		ec = _os9_open_parent_directory(&parent_path, pathlist,
						FAM_DIR | FAM_WRITE,
						&filename);

		if (ec != 0)
		{
			goto clean;
		}


		/* 1. Check if filename conforms to OS-9 standard. */

		ec = _os9_prsnam(filename);

		if (ec != 0)
		{
			goto clean;
		}


		/* 2. Check if file already exists. */

		ec = _os9_file_exists(parent_path, filename);

		if (ec == EOS_FAE && (mode & FAM_NOCREATE) == 0)
		{
			/* 1. If file already exists and FAM_NOCRFEATE isn't specified,
			 * delete the file and try to create it again.
			 */

			_os9_close(parent_path);
			parent_path = NULL;

			ec = _os9_delete(pathlist);

			if (ec == 0)
			{
				free(filename);
				filename = NULL;
				goto aa;
			}
		}

		if (ec != 0)
		{
			goto clean;
		}

		now = time(NULL);

		/* 3. Populate file descriptor. */

		memset(&newFD, 0, sizeof(fd_stats));

		newFD.fd_att = perms;
		_int2(0, newFD.fd_own);
		UnixToOS9Time(now, (char *) newFD.fd_dat);
		newFD.fd_lnk = 1;
		_int4(0, newFD.fd_siz);
		newFD.fd_creat[0] = newFD.fd_dat[0];
		newFD.fd_creat[1] = newFD.fd_dat[1];
		newFD.fd_creat[2] = newFD.fd_dat[2];


		/* 4. Allocate new cluster on 'sectors per cluster' boundary. */

		newLSN = _os9_getfreebit(parent_path->bitmap,
					 int3(parent_path->lsn0->dd_tot)) *
			parent_path->spc;

		if (newLSN < 0)
		{
			ec = EOS_DF;
			goto clean;
		}

		/* 5. If our cluster size > 1, add this cluster and leftover count
		 * to the segment list of the newly allocated file descriptor
		 */

		if (parent_path->spc > 1)
		{
			_int3(newLSN + 1, newFD.fd_seg[0].lsn);
			_int2(parent_path->spc - 1, newFD.fd_seg[0].num);
		}


		/* 6. Write file descriptor to image file. */

		_os9_lsn_fseek(parent_path, newLSN);
		fwrite(&newFD, 1, sizeof(fd_stats), parent_path->fd);

		memset(&newDEntry, 0, sizeof(os9_dir_entry));
		strcpy((char *) &(newDEntry.name), filename);
		free(filename);
		filename = NULL;
		CStringToOS9String((u_char *) & (newDEntry.name));
		_int3(newLSN, newDEntry.lsn);

		_os9_seek(parent_path, 0, SEEK_SET);


		/* 7. Add directory entry to the parent's directory. */

		while ((ec = _os9_gs_eof(parent_path)) == 0)
		{
			os9_dir_entry dentry;
			int mode = parent_path->mode;


			/* 1. Set up path temporarily as a directory so _os9_readdir won't fail. */

			parent_path->mode |= FAM_DIR | FAM_READ;

			ec = _os9_readdir(parent_path, &dentry);

			parent_path->mode = mode;

			if (ec != 0)
			{	/* Error */
				break;
			}

			if (dentry.name[0] == '\0')
			{
				_os9_seek(parent_path, -(int) sizeof(dentry),
					  SEEK_CUR);
				break;
			}
		}

		if (ec != 0 && ec != EOS_EOF)
		{
			goto clean;
		}

		/* 8. Write the directory entry back to the image. */

		ec = _os9_writedir(parent_path, &newDEntry);

		if (ec != 0)
		{
			_os9_delbit(parent_path->bitmap, newLSN, 1);
			goto clean;
		}

		ec = _os9_close(parent_path);
		parent_path = NULL;

		if (ec != 0)
		{		/* Error */
			goto clean;
		}

		return (_os9_open(path, pathlist, mode));

	clean:
		if (filename != NULL)
			free(filename);

		if (parent_path != NULL)
			_os9_close(parent_path);

		if (*path != NULL)
		{
			_os9_close(*path);
			*path = NULL;
		}

		return ec;
	}


//    return ec;
}



/*
 * _os9_open()
 *
 * Open a path to a file or directory.
 *
 * Legal pathnames are:
 *
 * 1. imagename,@     (considered to be a 'raw' open of the image)
 * 2. imagename,      (considered to be an open of the root directory)
 * 3. imagename,file  (considered to be a file or directory open within the image)
 * 4. imagename       (considered to be an error)
 *
 * The presence of a comma in the pathlist indicates that at the least, a non-native open will
 * be performed.
 */
error_code _os9_open(os9_path_id * path, char *pathlist, int mode)
{
	error_code ec = 0;
	*path = NULL;
	char *p;
	char *tmppathlist = NULL;


	/* 1. Strip off FAM_NOCREATE if passed -- irrelavent to _os9_open. */

	mode = mode & ~FAM_NOCREATE;


	/* 2. Allocate & initialize path descriptor. */

	ec = init_pd(path, mode);

	if (ec != 0)
	{
		return ec;
	}


	/* 3. Attempt to validate the pathlist. */

	ec = validate_pathlist(path, pathlist);

	if (ec != 0)
	{
		term_pd(*path);
		*path = NULL;

		return ec;
	}


	/* 4. Determine if disk is being open in raw mode. */

	if (pathlist[strlen(pathlist) - 1] == '@')
	{
		/* 1. Yes, raw mode. */

		(*path)->israw = 1;
	}
	else
	{
		/* 1. No, normal mode. */

		(*path)->israw = 0;
	}

	{
		/* 1. Open a path to the image file. */
		char *open_mode;

		if (mode & FAM_WRITE)
		{
			open_mode = "rb+";
		}
		else
		{
			open_mode = "rb";
		}

		(*path)->fd = fopen((*path)->imgfile, open_mode);

		if ((*path)->fd == NULL)
		{
			ec = UnixToCoCoError(errno);
			goto clean;
		}
	}


	/* 6. Read LSN0. */

	ec = init_lsn0(*path);

	if (ec != 0)
	{
		goto clean;
	}


	/* 7. Read bitmap. */

	ec = init_bitmap(*path);

	if (ec != 0)
	{
		goto clean;
	}


	/* 8. If path is raw, just return now. */

	if ((*path)->israw == 1)
	{
		return 0;
	}

	tmppathlist = NULL;


	/* 9. Walk the pathlist to find the FD LSN of the last element
	 * in the pathlist.
	 */

	(*path)->pl_fd_lsn = int3((*path)->lsn0->dd_dir);

	tmppathlist = strdup((*path)->pathlist);
	p = strtok(tmppathlist, "/");
	if (p == NULL)
	{
		p = ".";
	}
	do
	{
		os9_dir_entry diskent;


		for (;;)
		{
			char q[64];
			int mode = (*path)->mode;


			/* 1. Set up path temporarily as a directory so _os9_readdir won't fail. */

			(*path)->mode |= FAM_DIR | FAM_READ;

			ec = _os9_readdir(*path, &diskent);

			(*path)->mode = mode;

			if (ec != 0)
			{
				break;
			}


			/* 2. Try to match up the pathlist element to this entry. */

			strcpy(q, (char *) diskent.name);

			if (strcasecmp
			    ((char *) p,
			     (char *) OS9StringToCString((u_char *) q)) == 0)
			{
				(*path)->pl_fd_lsn = int3(diskent.lsn);
				(*path)->filepos = 0;
				break;
			}
		}
	}
	while (ec == 0 && (p = strtok(NULL, "/")) != 0);


	/* 10. If error encountered, return. */

	if (ec != 0)
	{
		if (ec == EOS_EOF)
		{
			ec = EOS_PNNF;
		}
		goto clean;
	}


	/* 11. Obtain fd sector and check file permissions against
	 * passed permissions.
	 *
	 * Note that we only check for owner read/write/dir permissions
	 * and ignore whether the user on the host OS has the same UID
	 * as the file's creator.  Those checks may come later.
	 */

	{
		fd_stats fd_sector;
		int andresult;


		_os9_lsn_fseek((*path), (*path)->pl_fd_lsn);
		fread(&fd_sector, 1, sizeof(fd_stats), (*path)->fd);

		/* 1. Check permissions to determine if we can access the file. */

		andresult =
			mode & (fd_sector.
				fd_att & (FAM_DIR | FAM_READ | FAM_WRITE));

		if (andresult != mode
		    || ((fd_sector.fd_att & FAM_DIR) != (mode & FAM_DIR)))
		{
			ec = EOS_FNA;
			goto clean;
		}
	}

	free(tmppathlist);

	return 0;

clean:
	if (tmppathlist != NULL)
	{
		free(tmppathlist);
	}

	if (*path != NULL)
	{
		_os9_close(*path);
		*path = NULL;
	}

	return ec;
}

error_code _os9_open_parent_directory(os9_path_id * path, char *pathlist,
				      int mode, char **filename)
{
	char *pathcopy, *lastPathComponent, *lastPathSeperator;

	/* 1. Generate path to parent. */
	DEBUG_PRINT("Initializing *path to NULL");
	*path = NULL;

	pathcopy = strdup(pathlist);

	lastPathComponent = strchr(pathcopy, ',') + 1;
	lastPathSeperator = strrchr(lastPathComponent, '/');

	if (lastPathSeperator != NULL)
	{
		lastPathComponent = lastPathSeperator + 1;
	}

	*filename = strdup(lastPathComponent);
//    strcpy(filename, lastPathComponent);
	pathcopy[lastPathComponent - pathcopy] = '.';
	pathcopy[lastPathComponent - pathcopy + 1] = '\0';

	error_code ec = _os9_open(path, pathcopy, mode);

	free(pathcopy);

	return ec;
}



/*
 * _os9_close()
 *
 * Close a path to a file
 */
error_code _os9_close(os9_path_id path)
{
	int pad_size, i;
	char pad = 0xff;

	if (path == NULL)
		return 0;

	if (path->fd != NULL)
	{
		/* 1. This is a valid path. */

		if (path->israw == 0)
		{
			_os9_truncate_seg_list(path);
		}

		term_bitmap(path);
		term_lsn0(path);

		/* 1. Make sure file length is an exact multiple of 256. */
		/* Extend file length if not */

		fseek(path->fd, 0, SEEK_END);
		pad_size = 256 - (ftell(path->fd) % 256);
		if (pad_size == 256)
		{
			pad_size = 0;
		}

		for (i = 0; i < pad_size; i++)
		{
			fwrite(&pad, 1, 1, path->fd);
		}

		fclose(path->fd);
		path->fd = NULL;
	}

	term_pd(path);

	return 0;
}



static void _os9_truncate_seg_list(os9_path_id path)
{
	fd_stats fd_sector;
	int i;
	unsigned int file_size, rounded_size, max_size, truncation;
	unsigned int clusters_to_truncate = 0;


	/* 1. Seek to file descriptor sector for this file and get it in memory. */

	_os9_lsn_fseek(path, path->pl_fd_lsn);
	fread(&fd_sector, 1, sizeof(fd_stats), path->fd);


	/* 2. If this file is a directory, then abort. */

	if ((fd_sector.fd_att & FAP_DIR) == FAP_DIR)
	{
		return;
	}


	/* 3. Get size of file. */

	file_size = int4(fd_sector.fd_siz);


	/* 4. Get number of bytes when rounded to next cluster. */

	rounded_size = NextHighestMultiple(file_size, path->cs);


	/* 5. Get number of bytes as currently represented by segment list. */

	max_size = _os9_maximum_file_size(fd_sector, path->bps);


	/* 6. If the size that the segment list conveys is larger than our round size,
	 * then compute the truncation value (how much we will cut).
	 */

	if (max_size > rounded_size)
	{
		truncation =
			_os9_maximum_file_size(fd_sector,
					       path->bps) - rounded_size;
		clusters_to_truncate = truncation / path->cs;
	}


	while (clusters_to_truncate--)
	{
		/* 1. Remove one cluster from end of segement list. */

		for (i = NUM_SEGS - 1; i >= 0; i--)
		{
			if (int3(fd_sector.fd_seg[i].lsn) != 0)
			{
				_os9_delbit(path->bitmap,
					    (int3(fd_sector.fd_seg[i].lsn) +
					     int2(fd_sector.fd_seg[i].num) -
					     1) / path->spc, 1);
				_int2(int2(fd_sector.fd_seg[i].num) -
				      path->spc, fd_sector.fd_seg[i].num);

				if (int2(fd_sector.fd_seg[i].num) == 0)
				{
					_int3(0, fd_sector.fd_seg[i].lsn);
				}
				break;
			}
		}
	}


	_os9_lsn_fseek(path, path->pl_fd_lsn);
	fwrite(&fd_sector, 1, sizeof(fd_stats), path->fd);


	return;
}



/* Return the maximum size of the file based on the state of the segments */
int _os9_maximum_file_size(fd_stats fd_sector, int bytesPerSector)
{
	int maximum_size = 0, i;


	for (i = 0; i < NUM_SEGS && int3(fd_sector.fd_seg[i].lsn) != 0; i++)
	{
		maximum_size +=
			int2(fd_sector.fd_seg[i].num) * bytesPerSector;
	}


	return maximum_size;
}



/*
 * validate_pathlist()
 *
 * Determines if the passed <image,path> pathlist is valid.
 *
 * Copies the image file and pathlist file portions into
 * the path descriptor.
 *
 * Valid pathlist examples:
 *	foo,/
 *	foo,.
 *	foo,/bar
 *	foo,bar
 */
int validate_pathlist(os9_path_id * path, char *pathlist)
{
	char *p;
	char *tmppathlist;


	if (strchr(pathlist, ',') == NULL)
	{
		return EOS_BPNAM;
	}


	/* 2. Check validity of pathlist. */

	tmppathlist = strdup(pathlist);

	p = strtok(tmppathlist, ",");

	if (p == NULL)
	{
		free(tmppathlist);

		return (EOS_BPNAM);
	}


	/* 3. Copy OS-9 pathlist element. */

	(*path)->imgfile = strdup(p);

	p = strtok(NULL, ",");

	if (p == NULL)
	{
		/* 1. There was nothing following the native/os9 delimiter, assume root. */


		(*path)->pathlist = strdup(".");
	}
	else
	{
		/* 1. If here, there is something following the native/os9 delimiter */
		/* skip over any leading slashes after the , delimiter */

		while (*p == '/')
		{
			p++;
		}

		(*path)->pathlist = strdup(p);
	}

	free(tmppathlist);


	return 0;
}



error_code _os9_file_exists(os9_path_id folder_path, char *filename)
{
	error_code ec = 0;
	os9_dir_entry dentry;


	_os9_seek(folder_path, 0, SEEK_SET);

	while (_os9_gs_eof(folder_path) == 0)
	{
		int mode = folder_path->mode;

		folder_path->mode |= FAM_DIR | FAM_READ;
		ec = _os9_readdir(folder_path, &dentry);
		folder_path->mode = mode;

		if (ec != 0)
		{
			return ec;
		}


		if (strcasecmp
		    (filename, (char *) OS9StringToCString(dentry.name)) == 0)
		{
			return EOS_FAE;
		}
	}


	return ec;
}



static int init_pd(os9_path_id * path, int mode)
{
	/* 1. Allocate path structure and initialize it. */

	*path = malloc(sizeof(struct _os9_path_id));
    printf("DEB: init_pd allocated path %p\n", *path);

	if (*path == NULL)
	{
		return 1;
	}


	/* 2. Clear out newly allocated path structure. */

	memset(*path, 0, sizeof(**path));

	(*path)->mode = mode;


	return 0;
}



static int term_pd(os9_path_id path)
{
	/* 1. Deallocate path structure. */
	if (path->imgfile != NULL)
	{
        printf("DEB: term_pd freeing imgfile %p\n", path->imgfile);
		free(path->imgfile);
	}

	if (path->pathlist != NULL)
	{
        printf("DEB: term_pd freeing pathlist %p\n", path->pathlist);
		free(path->pathlist);
	}

	free(path);


	return 0;
}



/*
 * init_bitmap()
 *
 * read the bitmap sectors
 */
static int init_bitmap(os9_path_id path)
{
	int bitmap_sectors;


	bitmap_sectors = (int2(path->lsn0->dd_map) / path->bps) +
		(int2(path->lsn0->dd_map) % path->bps != 0);
	path->bitmap_bytes = int2(path->lsn0->dd_map);
	path->spc = int2(path->lsn0->dd_bit);
	path->cs = path->spc * path->bps;	/* compute cluster size */

	path->bitmap = (u_char *) malloc(bitmap_sectors * path->bps);

	if (path->bitmap == NULL)
	{
		return 1;
	}

	_os9_lsn_fseek(path, 1);

	if (fread(path->bitmap, 1, bitmap_sectors * path->bps, path->fd) == 0)
	{
		return EOS_EOF;
	}


	return 0;
}


/*
 * term_bitmap()
 *
 * free the bitmap sectors
 */
static int term_bitmap(os9_path_id path)
{
	/* 1. Write back out bitmap. */

	_os9_lsn_fseek(path, 1);
	fwrite(path->bitmap, 1, path->bitmap_bytes, path->fd);

	free(path->bitmap);


	return 0;
}


/*
 * init_lsn0()
 *
 * Read LSN0, the first sector of an OS-9 disk
 */
static int init_lsn0(os9_path_id path)
{
	/* 1. Allocate 256 bytes for LSN0. */

	path->lsn0 = (lsn0_sect *) malloc(1 * 256);

	if (path->lsn0 == NULL)
	{
		return 1;
	}


	/* 2. Compute bytes per sector from value at LSN0. */

	_os9_lsn_fseek(path, 0);


	/* 3. Read 256 byte LSN0. */

	fread(path->lsn0, 1, 256, path->fd);


	/* 4. Compute bytes per sector from LSN0's lsnsize field. */

	if (int1(path->lsn0->dd_lsnsize) == 0)
	{
		/* 1. OS-9/6809 and some OS-9/68K formats have this field as 0,
		 * which means 256 bytes/sector.
		 */

		path->bps = 256;
	}
	else
	{
		/* 1. In this case, OS-9/68K has the proper value in
		 * the field (1 = 256 bps, 2 = 512 bps, etc.).
		 */

		path->bps = int1(path->lsn0->dd_lsnsize) * 256;
	}

	/* 5. Determine proper sectors per track */

	path->spt = int2(path->lsn0->dd_spt);
	path->t0s = int2(path->lsn0->pd_t0s);

	if (path->t0s == 0)
	{
		fprintf( stderr, "Warning: T0S is zero. Assuming SPT: %d\n", path->spt);
		path->t0s = path->spt;
	}

	return 0;
}



/*
 * term_lsn0()
 *
 * Terminate LSN0 buffer
 */
static int term_lsn0(os9_path_id path)
{
	free(path->lsn0);

	return 0;
}
