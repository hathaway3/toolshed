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

---

## dis68 (Disassembler)

A disassembler for OS-9/68K. It requires a parameter file to guide the disassembly process.

**Syntax:** `dis68 [options] <binary_file>`

---

## lst2cmt (MAME Comment Tool)

Converts `lwasm` listing files into MAME comment files.

**Syntax:** `lst2cmt {[<opts>]} <srcfile> <destfile>`

---

## makewav (WAV Converter)

Converts Motorola S-Records or raw binary files into CoCo/MC-10 audio WAV files.

**Syntax:** `makewav [options] <input-file>`

| Option | Description |
| --- | --- |
| `-l<val>` | Length for silent leader (default 2s) |
| `-s<val>` | Sample rate (default 11250) |
| `-r` | Treat input as raw binary |
| `-c` | Input has DECB header |
| `-o<file>` | Output WAV filename |

---

## tocgen (Sierra AGI TOC Generator)

A utility to generate Table of Contents files for Sierra AGI games.

**Syntax:** `tocgen {[<opts>]} {<infile>} {[<outfile>]}`

| Option | Description |
| --- | --- |
| `-q` | Quiet mode (suppress output) |

**Description:**
`tocgen` parses an input text file containing volume/disk/side mappings and creates a `tOC` file digestible by Sierra AGI game engines.
