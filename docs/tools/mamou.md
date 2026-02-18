# Mamou Assembler

The Mamou assembler is a cross assembler for OS-9/6809 programs. It is derived from the C port of Motorola's free 6809 assembler and includes functionality from Microware's `asm` assembler.

> [!NOTE]
> Mamou is included for historical purposes. For new development, it is recommended to use [LWTOOLS](http://lwtools.projects.l-w.ca/).

## Syntax
`mamou [<opts>] [file] [<opts>]`

## Options
- `-a<sym>[=<val>]`: assign val to sym
- `-b`: binary image file output
- `-c`: cross reference output
- `-d`: debug mode
- `-h`: Intel hex file output
- `-i[=]<dir>`: additional include directories
- `-l`: list file
- `-ls`: source only list
- `-o[=]<file>`: output to file
- `-p`: don't assemble, just parse
- `-s`: symbol table dump
