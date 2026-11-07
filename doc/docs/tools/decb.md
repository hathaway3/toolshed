# decb

The `decb` tool is used for manipulating Disk BASIC (RS-DOS) disk images (`.dsk`, `.dmk`, `.jvc`, `.vdk`, `.sdf`). For OS-9 images, see [os9](os9.md); for Dragon DOS images, see [dgndos](dgndos.md); for cassette images, see [cecb](cecb.md).

### Pathnames and Shortcodes
Pathnames for `decb` can be either native file paths or paths to files within a Disk BASIC image separated by a comma (e.g., `disk.dsk,PROGRAM.BAS`).

Issuing the `decb` command without any parameters will provide a list of available subcommands. Issuing a subcommand without any parameters (or with `-?`) will display help on using the subcommand. Every subcommand also accepts a two-letter shortcode:

| Subcommand | Shortcode | Subcommand | Shortcode |
| --- | --- | --- | --- |
| `attr` | `at` | `fstat` | `fs` |
| `binbust` | `bb` | `hdbconv` | `hc` |
| `copy` | `cp` | `info` | `in` |
| `dir` | `dr` | `kill` | `ki` |
| `dsave` | `ds` | `list` | `li` |
| `dskini` | `di` | `rename` | `mv` |
| `dump` | `du` | `reveal` | `rv` |
| `free` | `fr` | | |

## Subcommands

### ATTR (`at`) - Display or modify file attributes

**Syntax:** `attr {[<opts>]} {<file> [<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-0` | BASIC program |
| `-1` | BASIC data file |
| `-2` | Machine-language program |
| `-3` | Text file |
| `-a` | ASCII file |
| `-b` | Binary file |
| `-q` | quiet mode (suppress output) |

---

### BINBUST (`bb`) - Strip DECB binary headers and flatten segments

**Syntax:** `binbust {[<opts>]} <srcfile> <outfile> {[<opts>]}`

| Option | Description |
| --- | --- |
| `-r` | rewrite if output file exists |

**Description:**
Strips 5-byte DECB binary segment headers (`$00`, length, load address) and trailers (`$FF $00 $00`, exec address), flattening single- or multi-segment DECB machine-language binaries into a raw contiguous binary file (padding address gaps with `$00`). Works across native, DECB, OS-9, Dragon DOS, and CECB paths.

---

### COPY (`cp`) - Copy one or more files to a target directory

**Syntax:** `copy {[<opts>]} <srcfile> {[<...>]} <target> {[<opts>]}`

| Option | Description |
| --- | --- |
| `-[0-3]` | Set file type (0=BASIC, 1=Data, 2=Binary, 3=Source) |
| `-a` | Set data type to ASCII |
| `-b` | Set data type to binary |
| `-c` | Perform segment concatenation on machine language loadables |
| `-l` | Perform end-of-line translation |
| `-r` | Rewrite if file exists |
| `-t` | Perform BASIC token translation |

---

### DIR - Display the directory

**Syntax:** `dir {[<opts>]} {<dir> [<...>]} {[<opts>]}`

---

### DSAVE (`ds`) - Copy contents of a directory or device

**Syntax:** `dsave {[<opts>]} {[<source>]} <target> {[<opts>]}`

| Option | Description |
| --- | --- |
| `-b=size` | size of copy buffer in bytes or K-bytes |
| `-e` | actually execute commands |
| `-l` | perform end of line translation on copy |
| `-t` | perform BASIC token translation |
| `-r` | force rewrite on copy |
| `-x=path` | execute target path |

---

### DSKINI (`di`) - Create a Disk BASIC image

**Syntax:** `dskini {[<opts>]} <disk> {[<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-3` | 35 track disk (default) |
| `-4` | 40 track disk |
| `-8` | 80 track disk |
| `-f<fmt>` | disk image container format (`raw`, `jvc`, `vdk`, `dmk`, `sdf`; default inferred from extension or `raw`) |
| `-h<num>` | create <num> HDB-DOS drives |
| `-n<name>` | HDB-DOS disk name |
| `-s` | create a "skitzo" disk |

---

### DUMP (`du`) - Display the contents of a binary file

**Syntax:** `dump {[<opts>]} {<file> [<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-a` | dump output in assembler format (hex) |
| `-b` | dump output in assembler format (binary) |
| `-c` | don't display ASCII character data |
| `-e` | dump output in C format |
| `-h` | don't display header (DEPRECATED) |
| `-t` | don't display header |
| `-l` | don't display line label/count |
| `-z` | decode DECB or Dragon DOS binary headers |

---

### FREE (`fr`) - Display amount of free space on an image

**Syntax:** `free {[<opts>]} {<disk> [<...>]} {[<opts>]}`

---

### FSTAT (`fs`) - Display file status and granule allocation for a file

**Syntax:** `fstat {[<opts>]} {<file> [<...>]} {[<opts>]}`

---

### HDBCONV (`hc`) - Convert HDB-DOS disk image to 512-byte sector compatible

**Syntax:** `hdbconv {[<opts>]} <srcfile> {[<...>]} <target> {[<opts>]}`

| Option | Description |
| --- | --- |
| `-2` | go from 512-byte sector to 256-byte sector |
| `-5` | go from 256-byte sector to 512-byte sector (default) |

---

### INFO (`in`) - Display disk image container and filesystem information

**Syntax:** `info {[<opts>]} {<disk> [<...>]} {[<opts>]}`

**Description:**
Displays container format (`Raw`, `DMK`, `JVC`, `VDK`, `SDF`), geometry, write-protection status, Disk BASIC filesystem parameters (Track 17 directory/FAT layout, granule count, free granules/bytes, active file count), and any structural validation warnings.

---

### KILL (`ki`) - Remove files from a Disk BASIC image

**Syntax:** `kill {[<opts>]} {<file> [<...>]} {[<opts>]}`

---

### LIST (`li`) - Display the contents of a file

**Syntax:** `list {[<opts>]} {<file> [<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-t` | perform BASIC token translation |
| `-s` | perform S-Record encoding of binary |

---

### RENAME (`mv`) - Give a file a new filename

**Syntax:** `rename {<file> <newfilename>}`

---

### REVEAL (`rv`) - Explain disk image contents at a location

**Syntax:** `reveal [-b] <disk_image> [<location> [<end_location>]]`

| Option | Description |
| --- | --- |
| `-b` | interpret single numbers or `start..end` ranges as container byte offsets rather than LSNs |

**Location formats:**
- `<lsn>` or `<start_lsn> <end_lsn>` or `<start>..<end>`: Logical sector number(s) (default) or byte offset(s) (when `-b` is specified, or when `+<offset>` syntax is used).
- `T<track>S<sector>` or `T<track>H<head>S<sector>`: Physical track/head/sector coordinates (1-based sector).

**Description:**
Inspects a Disk BASIC image and explains what occupies the specified sector(s), granule(s), or byte offset(s), including Track 17 system sectors (boot sector, FAT, directory sectors), active file granule chains, deleted file remnants, or free granules.
