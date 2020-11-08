/********************************************************************
 * os9.c - OS-9 File Tools Executive
 *
 * $Id$
 ********************************************************************/
#include <util.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cocotypes.h>
#include <cocopath.h>


static void show_os9_help(char const * const *helpMessage);
static int do_command(int argc, char **argv);

/* Help message */
static char const * const helpMessage[] =
{
    "os9 from Toolshed " TOOLSHED_VERSION "\n",
    "Syntax: os9 {[<opts>]} <command> {[<opts>]}\n",
    "Usage:  OS-9 File Tools Executive\n",
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
    {os9attr,	"attr"},
    {os9cmp,	"cmp"},
    {os9copy,	"copy"},
    {os9dcheck,	"dcheck"},
    {os9del,	"del"},
    {os9deldir,	"deldir"},
    {os9dir,	"dir"},	
    {os9dsave,	"dsave"},	
    {os9dump,	"dump"},
    {os9format,	"format"},
    {os9free,	"free"},
    {os9fstat,	"fstat"},
    {os9gen,	"gen"},
    {os9id,	"id"},
    {os9ident,	"ident"},
    {os9list,	"list"},	
    {os9makdir,	"makdir"},
    {os9modbust,"modbust"},
    {os9padrom,	"padrom"},
    {os9rename,	"rename"},
    {NULL,	NULL}
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
                        show_os9_help(helpMessage);
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
        show_os9_help(helpMessage);
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
        fprintf(stderr, "os9: unknown command '%s'\n", argv[0]);
    }

    return(0);
}


static void show_os9_help(char const * const *helpMessage)
{
    char const * const *p = helpMessage;
    struct cmdtbl *ptr = table;
    char *argv[3];

    argv[0] = "os9";
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
