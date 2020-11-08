/********************************************************************
 * os9dsave.c - Copy utility for OS-9 filesystem
 *
 * $Id$
 ********************************************************************/
#include <util.h>
#include <string.h>
#include <cocotypes.h>
#include <cocopath.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <math.h>

/* globals */
static u_int buffer_size = 32768;

static error_code do_dsave(char *source, char *target, int execute, int buffsize, int rewrite);
static int DoFunc(int (*func)( int, char *[]), char *command);

/* Help message */
static char const * const helpMessage[] =
{
	"Syntax: dsave {[<opts>]} {[<source>]} <target> {[<opts>]}\n",
	"Usage: Copy the contents of a directory or device.\n",
	"Options:\n",
	"     -b=size    size of copy buffer in bytes or K-bytes\n",
	"     -e         actually execute commands\n",
	"     -r         force rewrite on copy\n",
	NULL
};


int os9dsave(int argc, char *argv[])
{
	error_code	ec = 0;
	char		*p = NULL, *q;
	int		i;
	int		count = 0;
	int		rewrite = 0;
	int		execute = 0;
	char		*target = NULL;
	char		*source = NULL;
	
	/* walk command line for options */
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			for (p = &argv[i][1]; *p != '\0'; p++)
			{
				switch(*p)
				{
					case 'b':
						if (*(++p) == '=')
						{
							p++;
						}
						q = p + strlen(p) - 1;
						if (toupper(*q) == 'K')
						{
							*q = '0';
							buffer_size = atoi(p) * 1024;
						}
						else
						{
							buffer_size = atoi(p);
						}
						p = q;
						break;
	
					case 'r':
						rewrite = 1;
						break;

					case 'e':
						execute = 1;
						break;

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

	/* Count non option arguments */
	for( i = 1, count = 0; i < argc; i++ )
	{
		if( argv[i] == NULL )
		{
			continue;
		}
		
		if( argv[i][0] == '-' )
		{
			continue;
		}
		
		if (source == NULL)
		{
			source = argv[i];
		}
		else if (target == NULL)
		{
			target = argv[i];
		}
		count++;
	}

	if (count < 1 || count > 2)
	{
		show_help(helpMessage);
		return(0);
	}

	/* if target is NULL, then source is really . and target is source */
	if (target == NULL)
	{
		target = source;
		source = ".";
	}

	/* do dsave */
	ec = do_dsave(source, target, execute, buffer_size, rewrite);
	if (ec != 0)
	{
		fprintf(stderr, "%s: error %d encountered during dsave\n", argv[0], ec);
	}

	return(ec);
}


static error_code do_dsave(char *source, char *target, int execute, int buffer_size, int rewrite)
{
	error_code	ec = 0;
	static int	level = 0;
	fd_dentry	dirent;
	char		command[1024];
	char		sourcePathList[1024];
	os9_path_id	sourcePath;

	ec = _os9_open(&sourcePath, source, FAM_DIR | FAM_READ);
	if (ec != 0)
	{
		return(ec);
	}

	/* read .. and . directories */
	_os9_readdir(sourcePath, &dirent);
	_os9_readdir(sourcePath, &dirent);
	
	while (_os9_readdir(sourcePath, &dirent) == 0 && dirent.name[0] != '\0')
	{
		os9_path_id	filePath;
		int		isdir = 1;

		OS9NameToString(dirent.name);

		sprintf(sourcePathList, "%s/%s", source, dirent.name);

		ec = _os9_open(&filePath, sourcePathList, FAM_DIR | FAM_READ);
		if (ec != 0)
		{
			isdir = 0;

			ec = _os9_open(&filePath, sourcePathList, FAM_READ);
			if (ec != 0)
			{
				_os9_close(sourcePath);

				return(ec);
			}
		}

		_os9_close(filePath);

		if (isdir == 1)
		{
			char newTarget[512];

			/* We've encountered a directory */
			newTarget[0] = '\0';

			/* 1. increment level indicator */
			level++;

			/* 2. make directory on target IF target path is relative */
			if (*target != '/')
			{
//				strcpy(newTarget, "../");
			}

			if (strcmp(target, "/") == 0)
			{
				sprintf(newTarget, "/%s", dirent.name);
			}
			else
			{
				sprintf(newTarget, "%s/%s", target, dirent.name);
			}

			/* 3. make directory on target */
			sprintf(command, "makdir %s", newTarget);
			puts(command);
			if (execute) 
			{
				DoFunc(os9makdir, command);
			}

			/* 4. call this function again */
			do_dsave(sourcePathList, newTarget, execute, buffer_size, rewrite);

			/* 5. decrement level indicator */
			level--;
		}
		else
		{
			/* We've encountered a file -- just copy */
			char ropt[4], bopt[32];

			ropt[0] = 0;
			bopt[0] = 0;

			if (buffer_size > 0)
			{
				sprintf(bopt, "-b=%d", buffer_size);
			}

			if (rewrite > 0)
			{
				strcat(ropt, "-r");
			}
			
			sprintf(command, "copy %s/%s %s/%s %s %s", source, dirent.name, target, dirent.name, ropt, bopt);
			puts(command);
			if (execute)
			{
				DoFunc(os9copy, command);
			}
		}
	}

	_os9_close(sourcePath);

	return(ec);
}


static int DoFunc(int (*func)(int, char *[]), char *command)
{
	error_code	ec = 0;
	char		*argv[64];
	char		*p;
	int		argc = 0;

	p = strtok(command, " ");
	argv[argc++] = p;
	do
	{
		p = strtok(NULL, " ");
		argv[argc++] = p;
	}
	while (p != NULL);
	argc--;

	(*func)(argc, argv);

	return(ec);
}
