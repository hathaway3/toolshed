# Other Utilities

This section covers various utilities included in the ToolShed project for archiving, disassembly, and file conversion.

## ar2 (Archiver)

`ar2` is an archiver utility modeled after the archive utility in Kernigan & Plauger's "Software Tools". It uses Lempel-Zev compression and preserves OS-9 attributes.

**Syntax:** `ar <-command>[options] <archive_name> [file_specification(s)]`

### Commands
| Command | Description |
| --- | --- |
| `d` | delete file(s) from the archive |
| `m` | move file(s) to the archive |
| `p` | print file(s) to standard output |
| `t` | print a table of contents |
| `u` | update/add file(s) to the archive |
| `x` | extract file(s) from the archive |

### Modifiers
| Option | Description |
| --- | --- |
| `a` | all versions (affect duplicate filenames or extract older versions) |
| `bnn` | set maximum compression bit size (`9`–`13`, default `11`) |
| `o` | allow overwriting existing files during extraction (`x`) |
| `s` | suppress customization of extracted binaries (`x`) |
| `ss` | read additional file specifications from standard input |
| `z` | store files uncompressed (`m`, `u`) |

---

## dis68 (Disassembler)

A 68000 disassembler for OS-9/68K modules. See [dis68](dis68.md) for full details.

**Syntax:** `dis68 [options] <binary_file>`

| Option | Description |
| --- | --- |
| `-a` | Produce output suitable for an assembler |
| `-c=<file>` | Specify the primary command/parameter file |
| `-o=<file>` | Redirect disassembled output to `<file>` |
| `-u` | Print unformatted output without address/hex columns |
| `-w=<width>` | Set page width for formatted listings |
| `-l=<length>` | Set page length for formatted listings |
| `-t` | Send output to standard output |

---

## lst2cmt (MAME Comment Tool)

Converts `lwasm` listing files into MAME debugger XML comment files. See [lst2cmt](lst2cmt.md) for full details.

**Syntax:** `lst2cmt {[<opts>]} <srcfile> <destfile>`

| Option | Description |
| --- | --- |
| `-b=size` | Size of copy buffer in bytes or K-bytes (default: 32K) |
| `-r` | Rewrite if destination file already exists |

---

## makewav (WAV Converter)

Converts Motorola S-Records or raw binary files into CoCo/MC-10 audio WAV or CAS files. See [makewav](makewav.md) for full details.

**Syntax:** `makewav [options] <input-file>`

| Option | Description |
| --- | --- |
| `-l<val>` | Length for silent leader (default 2s) |
| `-s<val>` | Sample rate (default 11250 Hz) |
| `-r` | Treat input as raw binary |
| `-c` | Input has DECB header |
| `-n<name>` | Filename to encode in cassette header |
| `-o<file>` | Output WAV/CAS filename |
| `-[0-2]` | File type (`0`=BASIC, `1`=Data, `2`=ML [default]) |
| `-[a\|b]` | Data type (`a`=ASCII, `b`=Binary [default]) |
| `-d<val>` | Start/load address |
| `-e<val>` | Execution address |
| `-g<val>` | Group `<val>` blocks with pause + leader |
| `-p<val>` | Pause duration in seconds between block groups |
| `-k` | Output in CAS format instead of WAV |
| `-v` | Verbose information |

---

## tocgen (CoCo SDC TOC Generator)

A utility to generate binary Table of Contents files for the CoCo SDC ROM launcher. See [tocgen](tocgen.md) for full details.

**Syntax:** `tocgen {[<opts>]} {<infile>} {[<outfile>]}`

| Option | Description |
| --- | --- |
| `-q` | Quiet mode (suppress progress output) |

**Description:**
`tocgen` parses an input text file containing title and command lines and creates a binary table-of-contents file (default `toc.bin`) for the CoCo SDC ROM launcher.
