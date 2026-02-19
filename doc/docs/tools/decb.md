# decb

The `decb` tool is used for manipulating Disk BASIC (RSDOS) disk images. For OS-9 images, see [os9](os9.md); for cassette images, see [cecb](cecb.md).

### General Options
| Option | Description |
| --- | --- |
| `-gX` | Set granule count (used for HDB-DOS and other large images) |

### Pathnames
Pathnames for `decb` can be either native file paths or paths to files within a Disk BASIC image.

Issuing the `decb` command without any parameters will provide a list of available subcommands. Issuing a subcommand without any parameters will display help on using the subcommand.

## Subcommands

### ATTR - Display or modify file attributes

**Syntax:** `attr {[<opts>]} {<file> [<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-0` | BASIC program |
| `-1` | BASIC data file |
| `-2` | Machine-language program |
| `-3` | Text file |
| `-a` | ASCII file |
| `-b` | Binary file |

---

### COPY - Copy one or more files to a target directory

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

### DSAVE - Copy contents of a directory or device

**Syntax:** `dsave {[<opts>]} {[<source>]} <target> {[<opts>]}`

| Option | Description |
| --- | --- |
| `-b=size` | size of copy buffer in bytes or K-bytes |
| `-e` | actually execute commands |
| `-l` | perform end of line translation on copy |
| `-t` | perform BASIC token translation |
| `-r` | force rewrite on copy |

---

### DSKINI - Create a Disk BASIC image

**Syntax:** `dskini {[<opts>]} <disk> {[<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-3` | 35 track disk (default) |
| `-4` | 40 track disk |
| `-8` | 80 track disk |
| `-h<num>` | create <num> HDB-DOS drives |
| `-n<name>` | HDB-DOS disk name |
| `-s` | create a "skitzo" disk |

---

### DUMP - Display the contents of a binary file

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
| `-z` | decode DECB binary |

---

### FREE - Display amount of free space on an image

**Syntax:** `free {[<opts>]} {<disk> [<...>]} {[<opts>]}`

---

### HDBCONV - Convert HDB-DOS disk image to 512-byte sector compatible

**Syntax:** `hdbconv {[<opts>]} <srcfile> {[<...>]} <target> {[<opts>]}`

| Option | Description |
| --- | --- |
| `-2` | go from 512-byte sector to 256-byte sector |
| `-5` | go from 256-byte sector to 512-byte sector (default) |

---

### KILL - Remove files from a Disk BASIC image

**Syntax:** `kill {[<opts>]} {<file> [<...>]} {[<opts>]}`

---

### LIST - Display the contents of a file

**Syntax:** `list {[<opts>]} {<file> [<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-t` | perform BASIC token translation |
| `-s` | perform S-Record encoding of binary |

---

### RENAME - Give a file a new filename

**Syntax:** `rename {<file> <newfilename>}`
