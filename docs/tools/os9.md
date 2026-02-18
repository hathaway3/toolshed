# os9

The `os9` tool is used to manipulate OS-9 formatted disk images (RBF file system).

## Subcommands

The built-in commands such as `dir`, `copy`, etc., must be executed from the `os9` executive, and any parameters for that command follow the command name.

### [ATTR](#attr)
Display or modify file attributes.

### [CMP](#cmp)
Compare the contents of two files.

### [COPY](#copy)
Copy one or more files to a target directory.

### [DCHECK](#dcheck)
Verify the file structure of an RBF disk image.

### [DEL](#del)
Delete one or more files.

### [DELDIR](#deldir)
Delete a directory and its contents.

### [DIR](#dir)
Display the contents of a directory.

### [DSAVE](#dsave)
Copy the contents of a directory or device.

### [DUMP](#dump)
Display the contents of a binary file.

### [FORMAT](#format)
Create a disk image of a given size and type.

### [FREE](#free)
Display the amount of free space on an image.

### [FSTAT](#fstat)
Display the file descriptor sector for a file.

### [GEN](#gen)
Prepare a disk image for booting.

### [ID](#id)
Display sector 0 of an image.

### [IDENT](#ident)
Display OS-9 module information.

### [LIST](#list)
Display contents of a text file.

### [MAKDIR](#makdir)
Create one or more directories.

### [MODBUST](#modbust)
Bust a single merged file of OS-9 modules into separate files.

### [PADROM](#padrom)
Pad a file to a specific length.

### [RENAME](#rename)
Give a file a new filename.

---

## ATTR <a name="attr"></a>

**Syntax:** `attr {[<opts>]} {<file> [<...>]} {[<opts>]}`

**Scope:** RBF disk image files only.

**Options:**
- `-q`: quiet mode (suppress output)
- `-e`: set execute permission
- `-w`: set write permission
- `-r`: set read permission
- `-s`: set single user bit
- `-p[ewr]`: set public execute, write or read permission
- `-n[ewr]`: unset execute, write or read permission
- `-np[ewr]`: unset public execute, public write or public read permission

**Description:**
Every file in the RBF file system possesses attributes. These attributes determine how a file can be accessed.

---

## CMP <a name="cmp"></a>

**Syntax:** `cmp {[<opts>]} <file1> <file2> {[<...>]} {[<opts>]}`

**Scope:** Disk BASIC, RBF disk image files, and host files.

**Description:**
`cmp` compares the contents of two files on a byte-by-byte basis.

---

## COPY <a name="copy"></a>

**Syntax:** `copy {[<opts>]} <srcfile> {[<...>]} <target> {[<opts>]}`

**Scope:** Disk BASIC, RBF disk image files, and host files.

**Options:**
- `-b=size`: size of copy buffer in bytes or K-bytes
- `-l`: perform end of line translation (use for text files only)
- `-o=id`: set file's owner as id
- `-r`: rewrite if file exists

---

## DCHECK <a name="dcheck"></a>

**Syntax:** `dcheck {[<opts>]} {<disk> [<...>]} {[<opts>]}`

**Scope:** RBF disk images only.

**Options:**
- `-s`: check number of directories and files
- `-b`: suppress listing of unused clusters
- `-p`: print pathlists of questionable clusters

---

## DEL <a name="del"></a>

**Syntax:** `del {[<opts>]} {<file> [<...>]} {[<opts>]}`

**Scope:** RBF disk images only.

---

## DELDIR <a name="deldir"></a>

**Syntax:** `deldir {[<opts>]} {<directory>} {[<opts>]}`

**Scope:** RBF disk image files only.

**Options:**
- `-q`: quiet mode (suppress interaction)

---

## DIR <a name="dir"></a>

**Syntax:** `dir {[<opts>]} {<dir> [<...>]} {[<opts>]}`

**Scope:** RBF disk images or host file system.

**Options:**
- `-a`: show all files
- `-e`: extended directory
- `-r`: recurse directories

---

## DSAVE <a name="dsave"></a>

**Syntax:** `dsave {[<opts>]} {[<source>]} <target> {[<opts>]}`

**Scope:** Disk BASIC, RBF disk image files, and host files.

---

## DUMP <a name="dump"></a>

**Syntax:** `dump {[<opts>]} {<file> [<...>]} {[<opts>]}`

**Scope:** Disk BASIC, RBF disk image files, and host files.

**Options:**
- `-a`: assembler format (hex)
- `-b`: assembler format (binary)
- `-c`: don't display ASCII data
- `-h`: don't display header
- `-l`: don't display line label

---

## FORMAT <a name="format"></a>

**Syntax:** `format {[<opts>]} <disk> {[<...>]} {[<opts>]}`

**Scope:** Host file system only.

**Options:**
- `-bsX`: bytes per sector (default = 256)
- `-cX`: cluster size
- `-e`: format entire disk (make full sized image)
- `-k`: make OS-9/68K LSN0
- `-nX`: disk name
- `-q`: quiet

---

## FREE <a name="free"></a>

**Syntax:** `free {[<opts>]} {<disk> [<...>]} {[<opts>]}`

**Scope:** RBF disk image files only.

---

## FSTAT <a name="fstat"></a>

**Syntax:** `fstat {[<opts>]} {<file> [<...>]} {[<opts>]}`

**Scope:** RBF disk image files only.

---

## GEN <a name="gen"></a>

**Syntax:** `gen {[<opts>]} {<disk_image>}`

**Scope:** RBF disk images only.

**Options:**
- `-b=bootfile`: bootfile to copy and link
- `-c`: CoCo disk
- `-d`: Dragon disk
- `-e`: Extended boot
- `-t=trackfile`: kernel trackfile to copy

---

## ID <a name="id"></a>

**Syntax:** `id {[<opts>]} {<disk> [<...>]} {[<opts>]}`

**Scope:** RBF disk images only.

---

## IDENT <a name="ident"></a>

**Syntax:** `ident {[<opts>]} {<file> [<...>]} {[<opts>]}`

**Scope:** Disk BASIC, RBF disk image files, and host files.

---

## LIST <a name="list"></a>

**Syntax:** `list {[<opts>]} {<file> [<...>]} {[<opts>]}`

**Scope:** RBF disk image files only.

---

## MAKDIR <a name="makdir"></a>

**Syntax:** `makdir {<dirname> [<...>]} `

**Scope:** RBF disk images only.

---

## MODBUST <a name="modbust"></a>

**Syntax:** `modbust {[<opts>]} {<file> [<...>]} {[<opts>]}`

**Scope:** Disk BASIC, RBF disk image files, and host files.

---

## PADROM <a name="padrom"></a>

**Syntax:** `padrom {[<opts>]} <padsize> {<file> [<...>]} {[<opts>]}`

---

## RENAME <a name="rename"></a>

**Syntax:** `rename {<file> <newfilename>}`

**Scope:** RBF disk image files only.
