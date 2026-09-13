/********************************************************************
 * seek.c - OS-9 Seek routine
 *
 * $Id$
 ********************************************************************/

#include <stdlib.h>
#include <string.h>

#include "cocotypes.h"
#include "os9path.h"


error_code _os9_seek(os9_path_id path, int pos, int mode)
{
	error_code ec = 0;

	if (path == NULL)
	{
		return EOS_BMODE;
	}

	if (path->israw == 1)
	{
		if (pos != 0 && mode == SEEK_SET)
		{
			if (path->t0s != path->spt)
			{
				return EOS_BMODE;
			}
		}

		fseek(path->fd, pos, mode);
	}
	else
	{
		switch (mode)
		{
		case SEEK_SET:
			path->filepos = pos;
			break;

		case SEEK_CUR:
			path->filepos = path->filepos + pos;
			break;

		case SEEK_END:
		{
			fd_stats fdbuf;
			ec = _os9_gs_fd(path, sizeof(fd_stats), &fdbuf);
			if (ec == 0)
			{
				path->filepos = int4(fdbuf.fd_siz) + pos;
			}
			break;
		}

		default:
			ec = EOS_BMODE;
			break;
		}
	}


	return ec;
}


/*
 * _os9_lsn_fseek()
 *
 * Seek to proper LSN accounting for short track zero.
 */

int _os9_lsn_fseek(os9_path_id path, int lsn)
{
	long offset;

	if (lsn >= path->t0s)
	{
		/* Skip past missing sectors */
		lsn += path->spt - path->t0s;
	}

	offset = lsn * path->bps;

	return fseek(path->fd, offset, SEEK_SET);
}
