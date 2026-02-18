# ar2 Archiver

Ar is a utility modeled loosely after the archive utility in "Software Tools" by Kernighan & Plauger. Its purpose is to gather files into a common file to save disk space and keep related files together.

## Syntax
`ar <-command>[options] <archive_name> [file_specification(s)]`

## Commands
- `d`: delete file(s) from the archive
- `m`: move file(s) to the archive (add and then delete)
- `p`: print file(s) to standard output
- `t`: print a table of contents for the archive
- `u`: update/add file(s) to the archive
- `x`: extract file(s) from the archive

## Options
- `a`: include all versions of specified files
- `bnn`: set compression to 'nn' bits maximum
- `o`: make archives compatible with old ar
- `s`: suppress compression of binaries
- `ss`: suppress compression of all files
- `z`: get names of files to process from standard input
