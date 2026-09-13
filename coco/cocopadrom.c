/********************************************************************
 * os9padrom.c - OS-9 ROM padding utility
 *
 * $Id$
 ********************************************************************/
#include <util.h>
#include <string.h>
#include <cocopath.h>
#include <cocotypes.h>


#define BUFFSIZ	256

static int do_padrom(char **argv, char *file, int padSize, char padChar);
// static int pow(int x, int p);


/* Help message */
static char const *const helpMessage[] = {
	"Syntax: padrom {[<opts>]} <padsize> {<file> [<...>]} {[<opts>]}\n",
	"Usage:  Pad a file to a specific length.\n",
	"Options:\n",
	"     -c[=]<n>    character to pad (n=dec, %bin, 0oct or $hex)\n",
	NULL
};


int os9padrom(int argc, char **argv)
{
	error_code ec = 0;
	char *p = NULL;
	int i;
	int padSize = 0;
	char padChar = '\xff';
	char *file = NULL;

	/* if no arguments, show help and return */
	if (argv[1] == NULL)
	{
		show_help(helpMessage);
		return (0);
	}

	/* walk command line for options */
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			for (p = &argv[i][1]; *p != '\0'; p++)
			{
				char *q;

				switch (*p)
				{
				case 'c':
					if (*(++p) == '=')
					{
						p++;
					}
					q = p + strlen(p) - 1;
					padChar = StrToInt(p);
					p = q;
					break;

				case '?':
				case 'h':
					show_help(helpMessage);
					return (0);

				default:
					fprintf(stderr,
						"%s: unknown option '%c'\n",
						argv[0], *p);
					return (0);
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
			if (padSize == 0)
			{
				padSize = StrToInt(argv[i]);
				if (padSize <= 0)
				{
					fprintf(stderr,
						"%s: illegal pad size\n",
						argv[0]);
					return (1);
				}
			}
			else
			{
				file = argv[i];
				do_padrom(argv, file, padSize, padChar);
			}
		}
	}

	return (ec);
}


static int do_padrom(char **argv, char *file, int padSize, char padChar)
{
	error_code ec = 0;
	os9_path_id path;
	int j;
	int fileSize;

	ec = _os9_open(&path, file, FAM_WRITE);
	if (ec != 0)
	{
		fprintf(stderr, "%s: failed to open '%s'\n", argv[0], file);
		return (ec);
	}

	ec = _os9_gs_size(path, &fileSize);
	if (ec != 0)
	{
		fprintf(stderr, "%s: cannot get file size of '%s'\n", argv[0],
			file);
		_os9_close(path);
		return (ec);
	}
	if (padSize <= fileSize)
	{
		fprintf(stderr, "%s: padrom size insufficient\n", argv[0]);
		_os9_close(path);
		return (1);
	}
	_os9_seek(path, fileSize, SEEK_SET);
	{
		char pad_buf[4096];
		u_int bytes_to_pad = padSize - fileSize;
		memset(pad_buf, padChar, sizeof(pad_buf));

		while (bytes_to_pad > 0)
		{
			u_int chunk = bytes_to_pad > sizeof(pad_buf) ? (u_int)sizeof(pad_buf) : bytes_to_pad;
			u_int size = chunk;

			ec = _os9_write(path, pad_buf, &size);
			if (ec != 0 || size == 0)
			{
				break;
			}
			bytes_to_pad -= size;
		}
	}

	_os9_close(path);

	return (0);
}
