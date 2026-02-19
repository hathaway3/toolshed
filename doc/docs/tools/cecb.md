# cecb

The `cecb` tool is used to manipulate Color BASIC and Micro Color BASIC formatted cassette images. It supports WAV, CAS, and C10 cassette file containers. For disk-based images, see [os9](os9.md) and [decb](decb.md).

## Options

| Option | Description |
| --- | --- |
| `-t %` | Set threshold to remove background noise (WAV) |
| `-f [n]` | Set FSK delineation frequency (WAV) |
| `-p [e|o]` | Set even or odd WAV parity |
| `-s [n]` | Start at sample/bit n |
| `-z` | Suggest MC10 mode |

## Subcommands

### BULKERASE - Create a cassette image

**Syntax:** `bulkerase {[<opts>]} <file> {[<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-s<num>` | Sample rate of WAV file (default: 22050) |
| `-b<num>` | Bits per sample of WAV file (default: 8) |
| `-l<num>` | Length of silence to record in WAV file (default: 0.5s) |

---

### DIR - Display the directory

**Syntax:** `dir {[<opts>]} {<dir> [<...>]} {[<opts>]}`

---

### FSTAT - Display file information

**Syntax:** `fstat {[<opts>]} {<file> [<...>]} {[<opts>]}`

---

### DUMP - Display contents of a binary file

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

### LIST - Display contents of a file

**Syntax:** `list {[<opts>]} {<file> [<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-t` | perform BASIC token translation |
| `-s` | perform S-Record encoding of binary |

---

### COPY - Copy one or more files to a target image

**Syntax:** `copy {[<opts>]} <srcfile> {[<...>]} <target> {[<opts>]}`

| Option | Description |
| --- | --- |
| `-[0-3]` | file type (0=BASIC, 1=Data, 2=Binary, 3=Source) |
| `-[a|b]` | data type (a=ASCII, b=Binary) |
| `-[g|n]` | gap flag (g=Gaps, n=No Gaps) |
| `-d<n>` | load address |
| `-e<n>` | execution address |
| `-l` | perform end of line translation |
| `-t` | perform BASIC entokenization of ASCII text |
| `-k` | perform BASIC detokenization of binary data |
| `-s` | perform S-Record encode of machine language loadables |
| `-f` | perform S-Record decode of ASCII text file |
| `-c` | perform segment concatenation on machine language loadables |
