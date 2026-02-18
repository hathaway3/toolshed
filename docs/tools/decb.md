# decb

The `decb` tool is used to manipulate RSDOS (Disk BASIC) formatted disk images.

## Executive Options

The `decb` executive has one option specified before the sub-command:

- `-g<num>`: Granule count in FAT (used when writing)
    - `68`: 35 track disk (default)
    - `78`: 40 track disk
    - `156`: 80 track disk

## Subcommands

### [ATTR](#attr)
Display or modify file attributes.

### [COPY](#copy)
Copy one or more files to a target directory.

### [DIR](#dir)
Display the directory of a Disk BASIC image.

### [DSAVE](#dsave)
Copy the contents of a directory or device.

### [DSKINI](#dskini)
Create a Disk BASIC image of a given size.

### [DUMP](#dump)
Display the contents of a binary file.

### [FREE](#free)
Display the number of free granules.

### [FSTAT](#fstat)
Display the file descriptor sector for a file.

### [HDBCONV](#hdbconv)
Converts an HDB-DOS disk image into a 512-byte sector compatible one.

### [KILL](#kill)
Remove files from a Disk BASIC image.

### [LIST](#list)
Display the contents of a file.

### [RENAME](#rename)
Give a file a new filename.

---

## ATTR <a name="attr"></a>

**Syntax:** `attr {[<opts>]} {<file> [<...>]} {[<opts>]}`

**Options:**
- `-0`: BASIC program
- `-1`: BASIC data file
- `-2`: Machine-language program
- `-3`: Text file
- `-a`: ASCII file
- `-b`: Binary file

---

## COPY <a name="copy"></a>

**Syntax:** `copy {[<opts>]} <srcfile> {[<...>]} <target> {[<opts>]}`

**Options:**
- `-[0-3]`: file type
- `-[a|b]`: data type (a = ASCII, b = binary)
- `-l`: perform end of line translation
- `-r`: rewrite if file exists
- `-t`: perform BASIC token translation
- `-c`: perform segment concatenation on machine language loadables

---

## DSKINI <a name="dskini"></a>

**Syntax:** `dskini {[<opts>]} <disk> {[<...>]} {[<opts>]}`

**Options:**
- `-3`: 35 track disk (default)
- `-4`: 40 track disk
- `-8`: 80 track disk
- `-h<num>`: create `<num>` HDB-DOS drives
- `-n<name>`: HDB-DOS disk name
- `-s`: create a "skitzo" disk
