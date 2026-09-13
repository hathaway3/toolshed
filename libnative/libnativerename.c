/********************************************************************
 * rename.c - Rename file entry for OS-9 filesystems
 *
 * $Id$
 ********************************************************************/
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "cocotypes.h"
#include "cococonv.h"
#include "cocosys.h"
#include "nativepath.h"



error_code _native_rename(char *pathlist, char *new_name)
{
	error_code ec = 0;
	char *a;
	size_t needed = strlen(pathlist) + strlen(new_name) + 2;
	char stack_dest[1024];
	char *dest = stack_dest;

	if (needed > sizeof(stack_dest))
	{
		dest = malloc(needed);
		if (dest == NULL)
		{
			return 1;
		}
	}

	/* Construct destination path name */

	a = strrchr(pathlist, '/');

	if (a == NULL)
	{
		strcpy(dest, new_name);
	}
	else
	{
		size_t prefix_len = a - pathlist + 1;
		memcpy(dest, pathlist, prefix_len);
		dest[prefix_len] = '\0';
		strcat(dest, new_name);
	}

	if (rename(pathlist, dest) != 0)
	{
		ec = errno;
	}

	if (dest != stack_dest)
	{
		free(dest);
	}

	return ec;
}
