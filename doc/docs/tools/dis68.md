# dis68

A 68000 disassembler for OS-9/68k modules and binary files.

## Usage

```bash
dis68 [options] infile [outfile]
```

## Options

- `-m`: Treat the file as a raw binary, not in OS-9 module format.
- `-o[=]nnnn`: Use a logical offset of `nnnn` (specified in hexadecimal).
- `-p name`: Use `name` for the parameter file.
- `-s`: Create assembler source output.

## Description

The `dis68` tool is a disassembler specifically designed for the Motorola 68000 CPU family. It is capable of parsing OS-9/68k modules to identify entry points and data sections, or it can be used on raw binary files with a specified offset. Combined with the `-s` flag, it can generate source code that can be reassembled.
