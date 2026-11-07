# os9

The `os9` tool is used to manipulate OS-9/RBF formatted disk images (`.dsk`, `.dmk`, `.jvc`, `.vdk`, `.sdf`). It is part of the ToolShed suite, alongside [decb](decb.md) (for Disk BASIC), [dgndos](dgndos.md) (for Dragon DOS), and [cecb](cecb.md) (for Cassette BASIC).

Issuing the `os9` command without any parameters will provide a list of available subcommands. Issuing a subcommand without any parameters (or with `-?`) will display help on using the subcommand. Every subcommand also accepts a two-letter shortcode:

| Subcommand | Shortcode | Subcommand | Shortcode |
| --- | --- | --- | --- |
| `attr` | `at` | `gen` | `ge` |
| `cmp` | `cm` | `id` | `id` |
| `copy` | `cp` | `ident` | `it` |
| `dcheck` | `dc` | `info` | `in` |
| `del` | `de` | `list` | `li` |
| `deldir` | `dd` | `makdir` | `md` |
| `dir` | `dr` | `modbust` | `mb` |
| `dsave` | `ds` | `padrom` | `pr` |
| `dump` | `du` | `rename` | `mv` |
| `format` | `fo` | `reveal` | `rv` |
| `free` | `fr` | | |
| `fstat` | `fs` | | |

## Subcommands

### ATTR - Display or modify file attributes

**Syntax:** `attr {[<opts>]} {<file> [<...>]} {[<opts>]}`

This command is intended for RBF disk image files only.

| Option | Description |
| --- | --- |
| `-q` | quiet mode (suppress output) |
| `-e` | set execute permission |
| `-w` | set write permission |
| `-r` | set read permission |
| `-s` | set single user bit |
| `-p[ewr]` | set public execute, write or read permission |
| `-n[ewrs]` | unset execute, write, read, or single user permission |
| `-np[ewr]` | unset public execute, public write or public read permission |

**Description:**
Every file in the RBF file system possesses attributes. These attributes determine how a file can be accessed.

**Examples:**
```text
os9 attr os9l2.dsk,startup
----r-wr
```

---

### CMP - Compare the contents of two files

**Syntax:** `cmp {[<opts>]} <file1> <file2> {[<...>]} {[<opts>]}`

**Description:**
Compares the contents of two files on a byte-by-byte basis.

**Examples:**
```text
os9 cmp file1 file2
```

---

### COPY (`cp`) - Copy one or more files to a target directory

**Syntax:** `copy {[<opts>]} <srcfile> {[<...>]} <target> {[<opts>]}`

| Option | Description |
| --- | --- |
| `-b=size` | size of copy buffer in bytes or K-bytes |
| `-l` | perform end of line translation |
| `-o=id` | set file's owner as id |
| `-a=attr` | set OS-9 file attributes on target (e.g. `-a=erpepr` or `-a=-w`) |
| `-r` | rewrite if file exists |

**Description:**
Creates an exact copy of a file on either an RBF disk image or on the host file system.

---

### DCHECK - Verify the file structure of an RBF disk image

**Syntax:** `dcheck {[<opts>]} {<disk> [<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-s` | check the number of directories and files |
| `-b` | suppress listing of unused clusters |
| `-p` | print pathlists of questionable clusters |

---

### DEL - Delete one or more files

**Syntax:** `del {[<opts>]} {<file> [<...>]} {[<opts>]}`

---

### DELDIR - Delete a directory and its contents

**Syntax:** `deldir {[<opts>]} {<directory>} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-q` | quiet mode (suppress interaction) |

---

### DIR - Display the contents of a directory

**Syntax:** `dir {[<opts>]} {<dir> [<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-a` | show all files |
| `-e` | extended directory |
| `-r` | recurse directories |

---

### DSAVE (`ds`) - Copy contents of a directory or device

**Syntax:** `dsave {[<opts>]} {[<source>]} <target> {[<opts>]}`

| Option | Description |
| --- | --- |
| `-b=size` | size of copy buffer in bytes or K-bytes |
| `-e` | actually execute commands |
| `-l` | perform end of line translation on copy |
| `-r` | force rewrite on copy |
| `-x=path` | execute target path |

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

### FORMAT (`fo`) - Create a disk image of a given size and type

**Syntax:** `format {[<opts>]} <disk> {[<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-bsX` | bytes per sector (default = 256) |
| `-cX` | cluster size |
| `-e` | format entire disk |
| `-fX` | disk image container format (`raw`, `jvc`, `vdk`, `dmk`, `sdf`; default inferred from extension or `raw`) |
| `-k` | make OS-9/68K LSN0 |
| `-nX` | disk name |
| `-q` | quiet mode; do not report format summary |

**Floppy Options:**
| Option | Description |
| --- | --- |
| `-4` | 48 tpi (default) |
| `-9` | 96 tpi |
| `-sa` | sector allocation size (SAS) |
| `-sd` | single density |
| `-dd` | double density (default) |
| `-ss` | single sided (default) |
| `-ds` | double sided |
| `-tX` | tracks (default = 35) |
| `-stX` | sectors per track (default = 18) |
| `-szX` | sectors for track 0 (default = 18) |
| `-iX` | interleave (default = 3) |
| `-dr` | format a Dragon disk |

**Hard Drive Options:**
| Option | Description |
| --- | --- |
| `-lX` | number of logical sectors (floppy options ignored) |

---

### FREE - Display amount of free space on an image

**Syntax:** `free {[<opts>]} {<disk> [<...>]} {[<opts>]}`

---

### FSTAT - Display file descriptor sector for a file

**Syntax:** `fstat {[<opts>]} {<file> [<...>]} {[<opts>]}`

---

### GEN (`ge`) - Prepare a disk image for booting

**Syntax:** `gen {[<opts>]} {<disk_image>}`

| Option | Description |
| --- | --- |
| `-b=bootfile` | bootfile to copy and link to the image (host file, `image,path`, or `image,` to read via LSN0 `dd_bt`) |
| `-c` | CoCo disk (default) |
| `-d` | Dragon disk |
| `-e` | Extended boot (fragmented) |
| `-t=trackfile` | kernel trackfile to copy to the image (host file, `image,path`, or `image,` to copy the boot track from another image) |
| `-lX` | Special boottrack/kerneltrack Start LSN |

---

### ID (`id`) - Display sector 0 of an image

**Syntax:** `id {[<opts>]} {<disk> [<...>]} {[<opts>]}`

---

### IDENT (`it`) - Display OS-9 module information

**Syntax:** `ident {[<opts>]} {<file> [<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-s` | short output |

---

### INFO (`in`) - Display disk image container and filesystem information

**Syntax:** `info {[<opts>]} {<disk> [<...>]} {[<opts>]}`

**Description:**
Displays container format (`Raw`, `DMK`, `JVC`, `VDK`, `SDF`), geometry, write-protection status, OS-9 RBF LSN0 metadata (disk name, total sectors, cluster size, disk ID), free space, and any structural validation warnings.

---

### LIST - Display contents of a text file

**Syntax:** `list {[<opts>]} {<file> [<...>]} {[<opts>]}`

---

### MAKDIR - Create one or more directories

**Syntax:** `makdir {<dirname> [<...>]} `

---

### MODBUST - Bust a merged file of modules

**Syntax:** `modbust {[<opts>]} {<file> [<...>]} {[<opts>]}`

---

### PADROM - Pad a file to a specific length

**Syntax:** `padrom {[<opts>]} <padsize> {<file> [<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-b` | place padding at the beginning of the file |
| `-c[=]<n>` | character to pad (n=dec, %bin, 0oct or $hex) |

---

### RDUMP - Display formatted dump of OS-9 .r and .l files

**Syntax:** `rdump {[<opts>]} <file> [<file>] {[<opts>]}`

| Option | Description |
| --- | --- |
| `-g` | add global definition info |
| `-r` | add reference info |
| `-o` | add reference and local offset info |
| `-a` | all of the above |

> [!NOTE]
> **Note on RDUMP**: This utility (`os9/os9rdump.c`) is for inspecting 6809 relocatable object files (`.r`) and library files (`.l`). It is retained in the `os9/` source tree, though it is not compiled into the default `os9` command table.

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
Inspects an OS-9 RBF disk image and translates LSNs, physical sectors, or container byte offsets into human-readable descriptions of what occupies that region (LSN0, allocation bitmap, boot track, bootfile, directory descriptors/data, active file descriptors/data, deleted file remnants, or free space), while flagging filesystem corruption or invalid sector references.
