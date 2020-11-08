/********************************************************************
 * cecbfstat.c - File status utility for Cassette BASIC
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
#include <cecbpath.h>

static int do_fstat(char **argv, char *p);

/* Help message */
static char const * const helpMessage[] =
{
	"Syntax: fstat {[<opts>]} {<file> [<...>]} {[<opts>]}\n",
	"Usage:  Display detailed information about a file.\n",
	"Options:\n",
	NULL
};


int cecbfstat(int argc, char *argv[])
{
	error_code ec=0;
	char *p = NULL;
	int i;


	/* 1. Walk command line for options. */

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
						show_help(helpMessage);
						return(0);
	
					default:
						fprintf(stderr, "%s: unknown option '%c'\n", argv[0], *p);
						return(0);
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

			ec = do_fstat(argv, p);

			if (ec != 0)
			{
				fprintf(stderr, "%s: error %d\n", p, ec);
			}

			printf("\n");
		}
	}

	if (p == NULL)
	{
		show_help(helpMessage);
		return(0);
	}


	return(0);
}
	

static int do_fstat(char **argv, char *p)
{
	error_code	ec = 0;
	_path_type path_type;
	coco_path_id path;
	u_char *buffer = NULL;
	u_int size;

	/* 1. Open a path to the device. */

	ec = _coco_open_read_whole_file( &path, p, FAM_READ, &buffer, &size );

	if( ec != 0 )
		return ec;
		
	ec = _coco_gs_pathtype(path, &path_type );
	
	if( ec != 0 )
		return ec;
	
	if( path_type == CECB )
	{
		cecb_path_id cecb_path = path->path.cecb;

		printf("File Information for %s\n", p);
		printf("  File type            : ");

		switch (cecb_path->dir_entry.file_type)
		{
			case 0:
				printf("BASIC Program\n");
				break;

			case 1:
				printf("Data\n");
				break;

			case 2:
				printf("M/L Program\n");
				break;

			case 3:
				printf("Text Editor Source\n");
				break;

			default:
				printf("0x%x\n", cecb_path->dir_entry.file_type);
				break;
		}
		
		printf("  Data type            : ");
		
		switch (cecb_path->dir_entry.ascii_flag)
		{
			case 0:
				printf("Binary\n");
				break;

			case 255:
				printf("ASCII\n");
				break;

			default:
				printf("0x%x\n", cecb_path->dir_entry.ascii_flag);
				break;
		}

		printf("  Gap type             : ");
		
		switch (cecb_path->dir_entry.gap_flag)
		{
			case 0:
				printf("No\n");
				break;

			case 255:
				printf("Yes\n");
				break;

			default:
				printf("0x%x\n", cecb_path->dir_entry.gap_flag);
				break;
		}
		
		printf( "  ML Load Address      : %d (0x%4.4x)\n",
					cecb_path->dir_entry.ml_load_address1 << 8 | cecb_path->dir_entry.ml_load_address2,
					cecb_path->dir_entry.ml_load_address1 << 8 | cecb_path->dir_entry.ml_load_address2 );

		printf( "  ML Execution Address : %d (0x%4.4x)\n",
					cecb_path->dir_entry.ml_exec_address1 << 8 | cecb_path->dir_entry.ml_exec_address2,
					cecb_path->dir_entry.ml_exec_address1 << 8 | cecb_path->dir_entry.ml_exec_address2 );

		printf( "             File Size : %d bytes (0x%4.4x)\n", size, size );
	}
	
	if( buffer != NULL )
		free(buffer);
		
	_coco_close(path);

	return ec;
}
