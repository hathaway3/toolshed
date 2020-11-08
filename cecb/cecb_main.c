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

#include <util.h>
#include <cecbpath.h>

static void show_cecb_help(char const * const *helpMessage);
static int do_command(int argc, char **argv);

/* Help message */
static char const * const helpMessage[] =
{
	"cecb from Toolshed " TOOLSHED_VERSION "\n",
	"Syntax: cecb {[<opts>]} <sub command> {[<opts>]}\n",
	"Usage:  Cassette BASIC File Tools Executive\n",
	"Options:\n",
	"     -t <%>    = Set threshold to remove background noise (for WAV files).\n",
	"     -f <n>    = Set bit delineation frequency (for WAV files).\n",
	"     -p <e|o>  = Set even or odd WAV file parity (for WAV files).\n",
	"     -s <n>    = Start at sample/bit n in WAV/CAS file.\n",
	"\n",
	"     % is a decimal number between 0 and 1.\n",
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
	{cecbdir,		"dir"},
	{cecbfstat,     "fstat"},
	{os9dump,		"dump"},
	{decblist,		"list"},
	{cecbbulkerase,	"bulkerase"},
	{cecbcopy,		"copy"},
	{NULL,			NULL}
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
						return(0);
						break;
					
					case 't':
						if( strlen(argv[i]) == 2 )
						{
							i++;
							cecb_threshold = strtod( argv[i], NULL );
						}
						else
							cecb_threshold = strtod( &(argv[i][2]), NULL );
						break;
					
					case 'f':
						if( strlen(argv[i]) == 2 )
						{
							i++;
							cecb_frequency = strtod( argv[i], NULL );
						}
						else
							cecb_frequency = strtod( &(argv[i][2]), NULL );
						break;
					
					case 's':
						if( strlen(argv[i]) == 2 )
						{
							i++;
							cecb_start_sample = strtol( argv[i], NULL, 0 );
						}
						else
							cecb_start_sample = strtol( &(argv[i][2]), NULL, 0 );
						break;
					
					case 'p':
						if( strlen(argv[i]) == 2 )
						{
							i++;
							if( argv[i][0] == 'e' )
								cecb_wave_parity = EVEN;
							else
								cecb_wave_parity = ODD;
						}
						else
						{
							if( argv[i][2] == 'e' )
								cecb_wave_parity = EVEN;
							else
								cecb_wave_parity = ODD;
						}
						break;
					
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
        fprintf(stderr, "cecb: unknown command '%s'\n", argv[0]);
    }

    return(0);
}


static void show_cecb_help(char const * const *helpMessage)
{
	char const * const *p = helpMessage;
	struct cmdtbl *ptr = table;
	char *argv[3];

	argv[0] = "cecb";
	argv[1] = "-?";
	argv[2] = NULL;

	while (*p)
	{
		fputs(*(p++), stderr);
	}

	printf("\nSub commands:\n");
	
	while (ptr->keyword != NULL)
	{
		printf("     %s\n", ptr->keyword);
		ptr++;
	}

	return;
}

