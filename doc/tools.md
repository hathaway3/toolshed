# ToolShed Tools

This document provides an overview and usage instructions for the various tools included in the ToolShed project.

## Core Disk & Cassette Tools

### [os9](ToolShed.md#os9)
The `os9` tool is used to manipulate OS-9/RBF formatted disk images. 
- **Subcommands**: `ATTR`, `CMP`, `COPY`, `DCHECK`, `DEL`, `DELDIR`, `DIR`, `DSAVE`, `DUMP`, `FORMAT`, `FREE`, `FSTAT`, `GEN`, `ID`, `IDENT`, `LIST`, `MAKDIR`, `MODBUST`, `PADROM`, `RENAME`.
- **Key Feature**: Supports the comma (`,`) syntax to differentiate between host and image paths.

### [decb](ToolShed.md#decb)
The `decb` tool is used to manipulate Disk BASIC (RSDOS) formatted disk images.
- **Subcommands**: `ATTR`, `COPY`, `DIR`, `DSAVE`, `DSKINI`, `DUMP`, `FREE`, `FSTAT`, `HDBCONV`, `KILL`, `LIST`, `RENAME`.
- **Special Support**: Includes `HDBCONV` for HDB-DOS compatibility.

### [cecb](ToolShed.md#cecb)
The `cecb` tool is used to manipulate Color BASIC and Micro Color BASIC cassette files.
- **Subcommands**: `BULKERASE`, `DIR`, `FSTAT`, `DUMP`, `LIST`, `COPY`.

---

## Assemblers & Disassemblers

### mamou
A 6809 cross-assembler derived from Motorola's free 6809 assembler, with enhancements for OS-9/6809.
- **Usage**: `mamou [options] <input-file>`
- **Note**: This tool is kept for historical purposes; `LWTOOLS` is recommended for modern development.

### [dis68](ToolShed.md#dis68)
A 6809 disassembler.

---

## Utilities

### makewav
Converts Motorola S-Records or raw binary files into CoCo/MC-10 audio WAV files, matching the standard cassette format.
- **Usage**: `makewav [options] <input-file>`
- **Key Options**:
    - `-l<val>`: Length for silent leader (default 2 seconds).
    - `-s<val>`: Sample rate for WAV file (default 11250).
    - `-r`: Treat input file as raw binary.
    - `-c`: Input file has DECB header.
    - `-o<file>`: Output WAV file name.

### [ar2](ToolShed.md#ar2)
An archiver utility for OS-9/6809 files, originally by Carl Kreider.

### [lst2cmt](ToolShed.md#lst2cmt)
Converts `lwasm` listing files into MAME comment files.

---

## FUSE Support

### cocofuse
Enables mounting CoCo disk images as local filesystems on Linux and macOS using FUSE.
- **Installation**: Requires `libfuse-dev` on Linux or `macfuse` via Homebrew on macOS.
- **Note**: Highly useful for seamless interaction between the host OS and disk images.
