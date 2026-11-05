/********************************************************************
 * cecb.c - Cassette BASIC Tools Executive
 *
 * $Id$
 ********************************************************************/
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <toolshed.h>

#include <util.h>
#include <cecbpath.h>

static void show_cecb_help(char const *const *helpMessage);
static int do_command(int argc, char **argv);

/* Help message */
static char const *const helpMessage[] = {
	"cecb from Toolshed " TOOLSHED_VERSION "\n",
	"Syntax: cecb {[<opts>]} <sub command> {[<opts>]}\n",
	"Usage:  Cassette BASIC File Tools Executive\n",
	"        Works with WAV, CAS and C10 files\n",
	"Options:\n",
	"     -r<real> = Set WAV FSK aspect ratio for autodetection. Default: 1.85\n",
	"     -t<%>    = Set WAV FSK aspect ratio threshold. Default: 0.3 (30%)\n",
	"     -n<%>    = Set near-zero threshold for WAV samples. Default: 0.01 (1%)\n",
	"     -s<n>    = Start at sample/bit <n> in the input file.\n",
	"     -z       = Suggest MC-10 mode.\n",
	"                 Use MC-10 WAV files and MicroColor BASIC tokens.\n",
	"\n",
	"     % is a decimal number between 0 and 1.\n",
	NULL
};


struct cmdtbl
{
	int (*func)(int, char **);
	char *keyword;
	char *shortcode;
	char *synopsis;
};


static struct cmdtbl table[] = {
	{cecbdir, "dir", "dr", "List the contents of a cassette image."},
	{cecbfstat, "fstat", "fs", "Display detailed file information."},
	{cocodump, "dump", "du", "Display a hexadecimal dump of a file."},
	{cecbdumpblock, "dumpblock", "db", "Display a hexadecimal dump of cassette blocks."},
	{decblist, "list", "li", "Display text or a tokenized BASIC program."},
	{cecbbulkerase, "bulkerase", "be", "Create blank cassette images."},
	{cecbcopy, "copy", "cp", "Copy files between images and native files."},
	{cecbleader, "leader", "le", "Analyze leaders in a WAV cassette image."},
	{NULL, NULL, NULL, NULL}
};


int main(int argc, char *argv[])
{
	error_code ec = 0;
	int i;
	char *p, *command = NULL;

	/* walk command line for options */
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			for (p = &argv[i][1]; *p != '\0'; p++)
			{
				switch (*p)
				{
				case 'h':
				case '?':
					show_cecb_help(helpMessage);
					return (0);
					break;

				case 'r':
					p++;
					if (*p == '=') p++;
					if (*p == '\0')
					{
						if (i + 1 < argc && argv[i + 1][0] != '-')
						{
							i++;
							p = argv[i];
						}
						else
						{
							fprintf(stderr, "%s: option -r requires an argument\n", argv[0]);
							return 1;
						}
					}
					cecb_ratio = strtod(p, NULL);
					p += strlen(p) - 1;
					break;

				case 't':
					p++;
					if (*p == '=') p++;
					if (*p == '\0')
					{
						if (i + 1 < argc && argv[i + 1][0] != '-')
						{
							i++;
							p = argv[i];
						}
						else
						{
							fprintf(stderr, "%s: option -t requires an argument\n", argv[0]);
							return 1;
						}
					}
					cecb_threshold = strtod(p, NULL);
					p += strlen(p) - 1;
					break;

				case 'n':
					p++;
					if (*p == '=') p++;
					if (*p == '\0')
					{
						if (i + 1 < argc && argv[i + 1][0] != '-')
						{
							i++;
							p = argv[i];
						}
						else
						{
							fprintf(stderr, "%s: option -n requires an argument\n", argv[0]);
							return 1;
						}
					}
					cecb_zero_adjust = strtod(p, NULL);
					p += strlen(p) - 1;
					break;

				case 's':
					p++;
					if (*p == '=') p++;
					if (*p == '\0')
					{
						if (i + 1 < argc && argv[i + 1][0] != '-')
						{
							i++;
							p = argv[i];
						}
						else
						{
							fprintf(stderr, "%s: option -s requires an argument\n", argv[0]);
							return 1;
						}
					}
					cecb_start_sample = strtol(p, NULL, 0);
					p += strlen(p) - 1;
					break;

				case 'z':
					cecb_suggest_mc10 =
						1;
					break;

				default:
					fprintf(stderr,
						"%s: unknown option '%c'\n",
						argv[0], *p);
					return (1);
				}
			}
		}
		else
		{
			command = argv[i];
			break;
		}

	}

	if (command == NULL)
	{
		show_cecb_help(helpMessage);
	}
	else
	{
		ec = do_command(argc - i, &argv[i]);
	}

	if (ec != 0 )
	{
		fprintf(stderr, "Error: %d - %s\n", ec, TSReportError(ec));
	}

	return (ec);
}


static int do_command(int argc, char **argv)
{
	struct cmdtbl *x = table;

	while (x->func != NULL)
	{
		if (strcmp(argv[0], x->keyword) == 0 || strcmp(argv[0], x->shortcode) == 0)
		{
			return (x->func(argc, argv));
		}
		x++;
	}

	if (x->func == NULL)
	{
		fprintf(stderr, "cecb: unknown command '%s'\n", argv[0]);
	}

	return (1);
}


static void show_cecb_help(char const *const *helpMessage)
{
	char const *const *p = helpMessage;
	struct cmdtbl *ptr = table;

	while (*p)
	{
		fputs(*(p++), stderr);
	}

	printf("\nSub commands:\n");

	while (ptr->keyword != NULL)
	{
		printf("     %-9s (%s)  %s\n", ptr->keyword, ptr->shortcode, ptr->synopsis);
		ptr++;
	}

	return;
}
