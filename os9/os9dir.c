/********************************************************************
 * os9dir.c - Directory utility for OS-9
 *
 * $Id$
 ********************************************************************/
#include <util.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cocotypes.h>
#include <cocopath.h>
#include <queue.h>


/* These are tweakable parameters which affect the output of a
 * non-extended directory listing.
 */
#define	DIR_COLWIDTH	16	/* width of a non-extended directory entry column */
#define	DIR_COLS		5	/* number of non-extended directory entries per line */

static int do_dir(char **argv, char *p);

/* globals */
static int extended = 0, dotfiles = 0, recurse = 0;

/* Help Message */
static char const * const helpMessage[] =
{
	"Syntax: dir {[<opts>]} {<dir> [<...>]} {[<opts>]}\n",
	"Usage:  Display the contents of a directory.\n",
	"Options:\n",
	"     -a    show all files\n",
	"     -e    extended directory\n",
	"     -r    recurse directories\n",
	NULL
};


int os9dir(int argc, char *argv[])
{
	error_code	ec = 0;
	char *p = NULL;
	int i;

	if (argv[1] == NULL)
	{
		show_help(helpMessage);
		return(0);
	}

	/* walk command line for options */
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			for (p = &argv[i][1]; *p != '\0'; p++)
			{
				switch(*p)
				{
					case 'a':
						dotfiles = 1;
						break;
	
					case 'e':
						extended = 1;
						break;
	
					case 'r':
						recurse = 1;
						break;
	
					case '?':
					case 'h':
						show_help(helpMessage);
						return(0);
	
					default:
						fprintf(stderr, "%s: unknown option '%c'\n", argv[0], *p);
						return(0);
				}
			}
		}
	}

	/* walk command line for pathnames */
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			continue;
		}
		else
		{
			p = argv[i];
		}

		ec = do_dir(argv, p);

		if (ec != 0)
		{
			fprintf(stderr, "%s: error %d opening '%s'\n", argv[0], ec, p);
			return(ec);
		}
	}

	return(0);
}


static int do_dir(char **argv, char *p)
{
	error_code	ec = 0;
	int col_count = 0;
	char os9pathlist[256];
	os9_path_id path;
	static int depth = 0;	/* recursion depth counter */
	NodeType q_head = NULL;
	char filepath[256];


	/* 1. Copy the passed pathlist into our own local buffer. */
	
	strcpy(os9pathlist, p);


	/* 2. If last char is ',', then add '.' */

	if( os9pathlist[strlen(os9pathlist) - 1] == ',' )
	{
		strcat(os9pathlist, "." );
	}
	else
	{
		/* 1. Determine if ',' is anywhere in the pathlist. */
		
		if (strchr(os9pathlist, ',') == NULL)
		{
			/* 1. Add it. */
			
			strcat(os9pathlist, ",");
		}
	}
	

	/* 3. Open a path to the device. */
	
retry:
	ec = _os9_open(&path, os9pathlist, FAM_READ | FAM_DIR);

	if (ec != 0)
	{
		/* 1. If we got EOS_FNA, this might be a file... if it is, see if we have ',.' at end. */
		
		if (ec == EOS_FNA && os9pathlist[strlen(os9pathlist) - 1] != '.' && os9pathlist[strlen(os9pathlist) - 2] != ',')
		{
			/* 1. We didn't have ',.' at the end... add it and try to reopen the file as a disk image. */
			
			strcat(os9pathlist, ",.");
			goto retry;
		}
		
		fprintf(stderr, "%s: error %d opening '%s'\n", argv[0], ec, os9pathlist);

		return(ec);
	}

	printf("\n                           Directory of %s\n", os9pathlist);


	/* 5. If directory is extended, put out header. */
	
	if (extended == 1)
	{
		printf(" Owner    Last modified    Attributes Sector Bytecount Name\n");
		printf("-------   ---------------  ---------- ------ --------- ----\n");
	}


	while (_os9_gs_eof(path) == 0)
	{
		int size;
		os9_dir_entry dentry;
		os9_path_id path2;
		fd_stats fdbuf;
		char *filename;

		size = sizeof(dentry);

		ec = _os9_readdir(path, &dentry);

		if (ec != 0)
		{
			break;
		}

		filename = strdup((char *)dentry.name);
		OS9StringToCString((u_char *)filename);
		if (filename[0] == '\0' || (filename[0] == '.' && dotfiles == 0))
		{
			/* skip over deleted entries & dot files */
			free(filename);
			continue;
		}

		if (extended == 1 || recurse == 1)
		{
			/* EXTENDED directory output */

			/* open path to file and get file desc */
			strcpy(filepath, os9pathlist);
			strcat(filepath, "/");
			strcat(filepath, filename);

			ec = _os9_open(&path2, filepath, FAM_READ);
			if( ec != 0 )
			{
				ec = _os9_open(&path2, filepath, FAM_DIR | FAM_READ);
				if( ec != 0 )
				{
					break;
				}
			}

			_os9_gs_fd(path2, sizeof(fd_stats), &fdbuf);
			_os9_close(path2);
	
			if (extended == 1)
			{
				/* print owner ID */
				printf("%3d.%-3d  ", fdbuf.fd_own[0], fdbuf.fd_own[1]);

				/* print last modified date/time */
				printf(" %04d/%02d/%02d %02d%02d   ", 1900 + fdbuf.fd_dat[0],
					fdbuf.fd_dat[1], fdbuf.fd_dat[2], fdbuf.fd_dat[3],
					fdbuf.fd_dat[4]);
		
				/* print attributes */
				{
					char attrs[9];

					OS9AttrToString(fdbuf.fd_att, attrs);
					printf("%s", attrs);
				}

				/* print fd sector, file size, filename */
				printf("%8X  %8d %s\n",
					int3(dentry.lsn),
					int4(fdbuf.fd_siz),
					filename );
			}
		}

		if (extended == 0)
		{
			/* REGULAR NON-EXTENDED directory output */
			int remainder;

			remainder = DIR_COLWIDTH - 1 - strlen(filename);

			/* print filename */
			printf("%s", filename);

			if (++col_count == DIR_COLS)
			{
				printf("\n");
				col_count = 0;
			}
			else
			{
				do
				{
					printf(" ");
				}
				while (remainder-- > 0);
			}
		}

		/* check directory attribute for recursion */
		if (recurse == 1 && (fdbuf.fd_att & (1 << 7)) != 0 &&
			/* we don't want to recurse on . and .. */
			strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0)
		{
			qAddNode(&q_head, filepath, strlen(filepath) + 1);

		}
	}

	/* necessary to properly terminate a non-extended directory output listing */
	if (extended == 0 && col_count <= DIR_COLS)
	{
		printf("\n");
	}

	{
		NodeType node = q_head;
		NodeType deadNode;

		while (node != NULL)
		{
			depth++;
			do_dir(argv, (char *)node->data);
			depth--;
			deadNode = node;
			node = qGetNextNode(node);
			qDeleteNode(&q_head, deadNode);
		}
	}

	_os9_close(path);

	return(0);
}
