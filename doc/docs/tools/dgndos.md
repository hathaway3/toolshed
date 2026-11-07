# dgndos

The `dgndos` tool is used for manipulating Dragon DOS formatted disk images (`.dsk`, `.vdk`, `.jvc`, `.dmk`, `.sdf`). For OS-9 images, see [os9](os9.md); for Disk BASIC (RS-DOS) images, see [decb](decb.md); for cassette images, see [cecb](cecb.md).

### Pathnames and Shortcodes

Pathnames for `dgndos` can be either native host file paths or paths to files within a Dragon DOS disk image separated by a comma (e.g., `dragon.vdk,HELLO.BAS`).

Issuing the `dgndos` command without any parameters will provide a list of available subcommands. Issuing a subcommand without any parameters (or with `-?`) will display help on using the subcommand. Every subcommand also accepts a two-letter shortcode:

| Subcommand | Shortcode | Summary |
| --- | --- | --- |
| `attr` | `at` | Display or modify file attributes |
| `copy` | `cp` | Copy one or more files to a target directory |
| `dir` | `dr` | Display the directory of a Dragon DOS image |
| `dsave` | `ds` | Generate or execute commands to copy a directory |
| `dskinit` | `di` | Create and format a Dragon DOS disk image |
| `dump` | `du` | Display the contents of a binary file |
| `free` | `fr` | Display the amount of free space on an image |
| `fstat` | `fs` | Display the file status and allocation for a file |
| `info` | `in` | Display disk image container and filesystem information |
| `kill` | `ki` | Delete one or more files from a Dragon DOS image |
| `list` | `li` | Display the contents of a text or BASIC file |
| `rename` | `mv` | Give a file a new filename |
| `reveal` | `rv` | Explain disk image contents at a sector, LSN, or byte offset |

---

## Subcommands

### ATTR (`at`) - Display or modify file attributes

**Syntax:** `attr {[<opts>]} {<file> [<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-q` | quiet mode (suppress output) |
| `-p` | set write-protect attribute |
| `-np` | unset write-protect attribute |

**Examples:**
```bash
dgndos attr dragon.vdk,HELLO.BAS
dgndos attr -p dragon.vdk,HELLO.BAS
```

---

### COPY (`cp`) - Copy one or more files to a target directory

**Syntax:** `copy {[<opts>]} <srcfile> {[<...>]} <target> {[<opts>]}`

| Option | Description |
| --- | --- |
| `-b=size` | size of copy buffer in bytes or K-bytes |
| `-l` | perform end-of-line translation (`0x0A` ↔ `0x0D`) |
| `-r` | rewrite if file exists |
| `-t` | perform Dragon BASIC token translation |
| `-d<addr>` | load address for binary header (decimal or `0x` hex) |
| `-e<addr>` | execution address for binary header (decimal or `0x` hex) |

**Description:**
Copies files between the host filesystem and Dragon DOS disk images (or between Dragon DOS images). When `-d` or `-e` is supplied while copying a native file to a Dragon DOS image, a 9-byte Dragon DOS binary header (`$55`, type `0x02`, load address, length, exec address, `$AA`) is prepended automatically.

---

### DIR (`dr`) - Display the directory

**Syntax:** `dir {[<opts>]} {<dir> [<...>]} {[<opts>]}`

---

### DSAVE (`ds`) - Copy contents of a directory or device

**Syntax:** `dsave {[<opts>]} {[<source>]} <target> {[<opts>]}`

| Option | Description |
| --- | --- |
| `-b=size` | size of copy buffer in bytes or K-bytes |
| `-e` | actually execute commands |
| `-l` | perform end-of-line translation on copy |
| `-r` | force rewrite on copy |
| `-x=path` | execute target path |

---

### DSKINIT (`di`) - Create a Dragon DOS disk image

**Syntax:** `dskinit {[<opts>]} <disk> {[<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-4` | 40 track disk (default) |
| `-8` | 80 track disk |
| `-1` | single-sided (18 sectors/track, default) |
| `-2` | double-sided (36 logical sectors/track) |
| `-f<fmt>` | disk image container format (`raw`, `dmk`, `jvc`, `vdk`, `sdf`; default is inferred from extension or `raw`) |

**Examples:**
```bash
dgndos dskinit -4 -1 game.vdk
dgndos dskinit -8 -2 -f=dmk archive.dmk
```

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

### FSTAT (`fs`) - Display file status and allocation for a file

**Syntax:** `fstat {[<opts>]} {<file> [<...>]} {[<opts>]}`

---

### INFO (`in`) - Display disk image container and filesystem information

**Syntax:** `info {[<opts>]} {<disk> [<...>]} {[<opts>]}`

**Description:**
Displays container format (`Raw`, `VDK`, `JVC`, `DMK`, `SDF`), geometry (tracks, heads, sectors/track, sector size), write-protection status, Dragon DOS filesystem geometry on Track 20, free space, active file count, and any validation warnings.

---

### KILL (`ki`) - Remove files from a Dragon DOS image

**Syntax:** `kill {[<opts>]} {<file> [<...>]} {[<opts>]}`

---

### LIST (`li`) - Display the contents of a file

**Syntax:** `list {[<opts>]} {<file> [<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-t` | perform Dragon BASIC token translation |

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
Inspects a Dragon DOS disk image and explains what occupies the specified sector(s) or byte offset(s), including Track 20 system sectors (superblock/bitmap sectors 1–2 and directory sectors 3–18), active file extents, deleted file remnants, or free space.
