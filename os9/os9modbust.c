/********************************************************************
 * os9modbust.c - OS-9 module buster utility
 *
 * $Id$
 ********************************************************************/
#include <util.h>
#include <cocotypes.h>
#include <cocopath.h>
#include <os9module.h>
#include <cococonv.h>


static int do_modbust(char **argv, char *filename);

/* Help message */
static char const *const helpMessage[] = {
	"Syntax: modbust {[<opts>]} {<file> [<...>]} {[<opts>]}\n",
	"Usage:  Bust a single merged file of OS-9 modules into separate files.\n",
	"Options:\n",
	NULL
};



int os9modbust(int argc, char **argv)
{
	error_code ec = 0;
	int i;
	char *p = NULL;


	if (argv[1] == NULL)
	{
		show_help(helpMessage);

		return 0;
	}


	/* 1. Walk command line for options. */

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			for (p = &argv[i][1]; *p != '\0'; p++)
			{
				switch (*p)
				{
				case '?':
				case 'h':
					show_help(helpMessage);

					return 0;

				default:
					fprintf(stderr,
						"%s: unknown option '%c'\n",
						argv[0], *p);

					return 1;
				}
			}
		}
	}


	/* 2. Walk command line for pathnames. */

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

		ec = do_modbust(argv, p);

		if (ec != 0)
		{
			fprintf(stderr, "%s: error %d opening file %s\n",
				argv[0], ec, p);

			return ec;
		}
	}

	if (p == NULL)
	{
		show_help(helpMessage);
		return 1;
	}

	return 0;
}



int modbust_osk(coco_path_id path)
{
	error_code ec;
	u_int size;
	coco_path_id path2 = NULL;
	char name[256];
	int nameoffset;
	char buffer[256];
	u_char *module = NULL;
	coco_file_stat fstat;
	int namelen = 0;

	/* We have an OS-9 module - get 2 bytes then 4 byte size */
	size = 6;

	ec = _coco_read(path, buffer, &size);
	if (ec != 0 || size < 6)
	{
		return 1;
	}
	size = int4((u_char *) buffer + 2);
	if (size < 16)
	{
		return 1;
	}
	module = (u_char *) malloc(size);

	if (module == NULL)
	{
		printf("Memory allocation error\n");
		return (1);
	}

	module[0] = OSK_ID0;
	module[1] = OSK_ID1;
	module[2] = buffer[0];
	module[3] = buffer[1];
	module[4] = buffer[2];
	module[5] = buffer[3];
	module[6] = buffer[4];
	module[7] = buffer[5];
	u_int read_size = size - 8;
	ec = _coco_read(path, &module[8], &read_size);
	if (ec != 0)
	{
		free(module);
		return ec;
	}
	nameoffset = int4(&module[12]);
	if (nameoffset < 16 || (u_int)nameoffset >= size)
	{
		free(module);
		return 1;
	}
	namelen = 0;
	while ((u_int)(nameoffset + namelen) < size && namelen < (int)sizeof(name) - 1)
	{
		namelen++;
		if (module[nameoffset + namelen - 1] & 0x80)
			break;
	}
	if (namelen == 0 || !(module[nameoffset + namelen - 1] & 0x80))
	{
		free(module);
		return 1;
	}
	memcpy(name, &module[nameoffset], namelen);
	name[namelen] = '\0';
	OS9StringToCString((u_char *) name);
	printf("Busting module %s...\n", name);

	fstat.perms = FAP_READ | FAP_WRITE;
	ec = _coco_create(&path2, name, FAM_WRITE, &fstat);

	if (ec != 0)
	{
		printf("Error creating file %s\n", name);
		free(module);
		return (1);
	}

	u_int write_size = size;
	_coco_write(path2, module, &write_size);
	_coco_close(path2);
	path2 = NULL;

	free(module);

	return 0;
}

int modbust_os9(coco_path_id path)
{
	error_code ec;
	u_int size;
	coco_path_id path2 = NULL;
	char name[256];
	int nameoffset;
	char buffer[256];
	u_char *module = NULL;
	coco_file_stat fstat;
	int namelen = 0;

	/* We have an OS-9 module - get size */
	size = 2;

	ec = _coco_read(path, buffer, &size);
	if (ec != 0 || size < 2)
	{
		return 1;
	}
	size = int2((u_char *) buffer);
	if (size < 9)
	{
		return 1;
	}
	module = (u_char *) malloc(size);

	if (module == NULL)
	{
		printf("Memory allocation error\n");
		return (1);
	}

	module[0] = 0x87;
	module[1] = 0xCD;
	module[2] = buffer[0];
	module[3] = buffer[1];
	u_int read_size = size - 4;
	ec = _coco_read(path, &module[4], &read_size);
	if (ec != 0)
	{
		free(module);
		return ec;
	}
	nameoffset = int2(&module[4]);
	if (nameoffset < 4 || (u_int)nameoffset >= size)
	{
		free(module);
		return 1;
	}
	namelen = 0;
	while ((u_int)(nameoffset + namelen) < size && namelen < (int)sizeof(name) - 1)
	{
		namelen++;
		if (module[nameoffset + namelen - 1] & 0x80)
			break;
	}
	if (namelen == 0 || !(module[nameoffset + namelen - 1] & 0x80))
	{
		free(module);
		return 1;
	}
	memcpy(name, &module[nameoffset], namelen);
	name[namelen] = '\0';
	OS9StringToCString((u_char *) name);
	printf("Busting module %s...\n", name);

	fstat.perms = FAP_READ | FAP_WRITE;
	ec = _coco_create(&path2, name, FAM_WRITE, &fstat);

	if (ec != 0)
	{
		printf("Error creating file %s\n", name);
		free(module);
		return (1);
	}

	u_int write_size = size;
	_coco_write(path2, module, &write_size);
	_coco_close(path2);
	path2 = NULL;

	free(module);

	return 0;
}

static int do_modbust(char **argv, char *filename)
{
	error_code ec = 0;
	char buffer[256];
	coco_path_id path = NULL;

	ec = _coco_open(&path, filename, FAM_READ);

	if (ec != 0)
	{
		return ec;
	}

	while (_coco_gs_eof(path) == 0)
	{
		u_int size = 2;

		ec = _coco_read(path, buffer, &size);
		if (ec != 0)
		{
			if (ec != EOS_EOF)
			{
				fprintf(stderr, "%s: error reading file %s\n",
					argv[0], filename);
			}
			_coco_close(path);
			path = NULL;
			return (ec == EOS_EOF ? 0 : ec);
		}
		if (size < 2)
		{
			break;
		}

		if (buffer[0] == (char) OS9_ID0
		    && buffer[1] == (char) OS9_ID1)
		{
			modbust_os9(path);
		}
		else if (buffer[0] == (char) OSK_ID0
			 && buffer[1] == (char) OSK_ID1)
		{
			modbust_osk(path);
		}
	}

	_coco_close(path);
	path = NULL;

	return 0;
}
