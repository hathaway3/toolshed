# ToolShed Tools

This document provides an overview and usage instructions for the various tools included in the ToolShed project.

## Core Disk & Cassette Tools

All four executive tools (`os9`, `decb`, `dgndos`, `cecb`) accept both full subcommand names and two-letter shortcodes (for example, `dr` for `dir`, `cp` for `copy`, `fs` for `fstat`, `in` for `info`, `rv` for `reveal`).

### [os9](docs/tools/os9.md)
The `os9` tool is used to manipulate OS-9/RBF formatted disk images. 
- **Subcommands**: `ATTR` (`at`), `CMP` (`cm`), `COPY` (`cp`), `DCHECK` (`dc`), `DEL` (`de`), `DELDIR` (`dd`), `DIR` (`dr`), `DSAVE` (`ds`), `DUMP` (`du`), `FORMAT` (`fo`), `FREE` (`fr`), `FSTAT` (`fs`), `GEN` (`ge`), `ID` (`id`), `IDENT` (`it`), `INFO` (`in`), `LIST` (`li`), `MAKDIR` (`md`), `MODBUST` (`mb`), `PADROM` (`pr`), `RENAME` (`mv`), `REVEAL` (`rv`).
- **Key Feature**: Supports the comma (`,`) syntax to differentiate between host and image paths, and works transparently across raw, DMK, JVC, VDK, and SDF disk containers.

### [decb](docs/tools/decb.md)
The `decb` tool is used to manipulate Disk BASIC (RS-DOS) formatted disk images.
- **Subcommands**: `ATTR` (`at`), `BINBUST` (`bb`), `COPY` (`cp`), `DIR` (`dr`), `DSAVE` (`ds`), `DSKINI` (`di`), `DUMP` (`du`), `FREE` (`fr`), `FSTAT` (`fs`), `HDBCONV` (`hc`), `INFO` (`in`), `KILL` (`ki`), `LIST` (`li`), `RENAME` (`mv`), `REVEAL` (`rv`).
- **Special Support**: Includes `HDBCONV` for HDB-DOS compatibility and `BINBUST` for flattening segmented DECB binaries.

### [dgndos](docs/tools/dgndos.md)
The `dgndos` tool is used to manipulate Dragon DOS formatted disk images.
- **Subcommands**: `ATTR` (`at`), `COPY` (`cp`), `DIR` (`dr`), `DSAVE` (`ds`), `DSKINIT` (`di`), `DUMP` (`du`), `FREE` (`fr`), `FSTAT` (`fs`), `INFO` (`in`), `KILL` (`ki`), `LIST` (`li`), `RENAME` (`mv`), `REVEAL` (`rv`).
- **Key Feature**: Supports raw, VDK, JVC, DMK, and SDF disk containers and automatic Dragon BASIC tokenization/detokenization.

### [cecb](docs/tools/cecb.md)
The `cecb` tool is used to manipulate Color BASIC and Micro Color BASIC cassette files (`.cas`, `.c10`, `.wav`).
- **Subcommands**: `BULKERASE` (`be`), `COPY` (`cp`), `DIR` (`dr`), `DUMP` (`du`), `DUMPBLOCK` (`db`), `FSTAT` (`fs`), `LEADER` (`le`), `LIST` (`li`).

---

## Assemblers & Disassemblers

### [mamou](docs/tools/mamou.md)
A 6809 cross-assembler derived from Motorola's free 6809 assembler, with enhancements for OS-9/6809.
- **Usage**: `mamou [options] <input-file>`
- **Note**: This tool is deprecated and kept for historical purposes; `LWTOOLS` (`lwasm`) is recommended for modern development.

### [casm](docs/tools/casm.md)
A C-based 6809/6309 cross-assembler.

### [dis68](docs/tools/dis68.md)
A 68000 (OS-9/68K) disassembler by Dan Poole and Carl Kreider.

---

## Utilities

### [makewav](docs/tools/makewav.md)
Converts Motorola S-Records or raw binary files into CoCo/MC-10 audio WAV files, matching the standard cassette format.
- **Usage**: `makewav [options] <input-file>`
- **Key Options**:
    - `-l<val>`: Length for silent leader (default 2 seconds).
    - `-s<val>`: Sample rate for WAV file (default 11250).
    - `-r`: Treat input file as raw binary.
    - `-c`: Input file has DECB header.
    - `-o<file>`: Output WAV file name.

### [ar2](docs/tools/utilities.md#ar2)
An archiver utility for OS-9/6809 files, originally by Carl Kreider.

### [lst2cmt](docs/tools/lst2cmt.md)
Converts `lwasm` listing files into MAME debugger comment files.

### [tocgen](docs/tools/tocgen.md)
Generates binary table-of-contents files for the CoCo SDC ROM launcher.

---

## FUSE Support

### [cocofuse](docs/tools/cocofuse.md)
Enables mounting CoCo and Dragon disk images (OS-9, Disk BASIC, and Dragon DOS) as local filesystems on Linux and macOS using FUSE.
- **Installation**: Requires `libfuse-dev` on Linux or `macFUSE` / `fuse-t` via Homebrew on macOS.
- **Note**: Highly useful for seamless interaction between the host OS and disk images.
