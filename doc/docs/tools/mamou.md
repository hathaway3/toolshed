# mamou (6809/6309 Assembler)

`mamou` is a 6809 and 6309 cross-assembler. While historically significant and still used for building some firmware in the ToolShed repository (like SuperDOS), for new projects it is recommended to use [LWTOOLS](http://lwtools.projects.l-w.ca).

**Note:** `mamou` is officially deprecated in favor of `lwasm`.

!!! warning "Deprecated Tool"
    The `mamou` assembler is deprecated and has been removed from the default build. Users are strongly encouraged to migrate to [LWTOOLS](http://lwtools.projects.l-w.ca) (`lwasm`) for all future 6809/6309 assembly projects. `mamou` is no longer maintained.

## Usage
`mamou {[<opts>]} <files> {[<opts>]}`

### General Options
| Option | Description |
| --- | --- |
| `-D<sym>[=<val>]` | Assign value to symbol |
| `-d` | Debug mode |
| `-e` | Enhanced 6309 assembler mode |
| `-ee` | Enhanced 6309 and X9 assembler mode |
| `-I<dir>` | Additional include directories |
| `-p` | Don't assemble, just parse |
| `-q` | Quiet mode |
| `-x` | Suppress warnings and errors |
| `-y` | Include instruction cycle count |
| `-z` | Suppress conditionals in assembly list output |

### Source Listing Options
| Option | Description |
| --- | --- |
| `-c` | Show symbol cross reference table |
| `-l` | List file |
| `-ls` | Source only list (no line numbers) |
| `-ln` | Format source in 'new style' assembly |
| `-lt` | Use tabs instead of spaces |
| `-lu` | Force pseudo-ops to print in uppercase |
| `-np` | Suppress 'page' pseudo output |
| `-o<file>` | Output to file |
| `-s` | Show symbol table (multi-column format) |
| `-sa` | Show symbol table (assembler format) |

### Assembler Modes
Select only one:
| Option | Description |
| --- | --- |
| `-m9` | OS-9/6809 (default) |
| `-mm` | Microware RMA |
| `-mb` | Disk BASIC (short form `-b`) |
| `-mr` | ROM Absolute (short form `-r`) |

### Object Generation Options
Select only one:
| Option | Description |
| --- | --- |
| `-tb` | Binary object output (default) |
| `-th` | Hex object output |
| `-ts` | S-record object output |
