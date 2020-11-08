/********************************************************************
 * decb.c - Disk BASIC Tools Executive
 *
 * $Id$
 ********************************************************************/
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "util.h"


static void show_decb_help(char const * const *helpMessage);
static int do_command(int argc, char **argv);

/* Help message */
static char const * const helpMessage[] =
{
	"decb from Toolshed " TOOLSHED_VERSION "\n",
	"Syntax: decb {[<opts>]} <command> {[<opts>]}\n",
	"Usage:  Disk BASIC File Tools Executive\n",
	"Options:\n",
	NULL
};


struct cmdtbl
{
	int(*func)(int, char **);
	char *keyword;
	char *synopsis;
};


static struct cmdtbl table[] =
{
	{decbattr,	"attr"},
	{decbcopy,	"copy"},
	{decbdir,	"dir"},
	{decbdsave,     "dsave"},
	{decbdskini,	"dskini"},
	{os9dump,       "dump"},
	{decbfree,	"free"},
	{decbfstat,	"fstat"},
	{decbhdbconv,	"hdbconv"},
	{decbkill,	"kill"},
	{decblist,	"list"},
	{decbrename,	"rename"},
	{NULL,		NULL}
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
						show_decb_help(helpMessage);
						return(0);
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
		show_decb_help(helpMessage);
	}
	else
	{
		ec = do_command(argc - i, &argv[i]);
	}

	return(ec);
}


static int do_command(int argc, char **argv)
{
    struct cmdtbl *x = table;
    
    while (x->func != NULL)
    {
        if (strcmp(argv[0], x->keyword) == 0)
        {
            return(x->func(argc, argv));
        }
        x++;
    }

    if (x->func == NULL)
    {
        fprintf(stderr, "decb: unknown command '%s'\n", argv[0]);
    }

    return(0);
}


static void show_decb_help(char const * const *helpMessage)
{
	char const * const *p = helpMessage;
	struct cmdtbl *ptr = table;
	char *argv[3];

	argv[0] = "decb";
	argv[1] = "-?";
	argv[2] = NULL;

	while (*p)
	{
		fputs(*(p++), stderr);
	}

	printf("\nCommands:\n");
	
	while (ptr->keyword != NULL)
	{
		printf("     %s\n", ptr->keyword);
		ptr++;
	}

	return;
}

