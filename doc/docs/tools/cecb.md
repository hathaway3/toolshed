# cecb

The `cecb` tool is used to manipulate Color BASIC and Micro Color BASIC formatted cassette images. It supports WAV, CAS, and C10 cassette file containers. For disk-based images, see [os9](os9.md), [decb](decb.md), and [dgndos](dgndos.md).

## Executive Options

These options are passed before the subcommand (`cecb [<opts>] <command> ...`):

| Option | Description |
| --- | --- |
| `-r<real>` | Set threshold `0 < r < 1.0` to analyze WAV input (default: `0.95`) |
| `-t<%>` | Set threshold percentage for bit boundary variation in WAV |
| `-n<%>` | Set noise floor percentage to ignore in WAV |
| `-s<n>` | Start at sample/bit `n` in WAV/CAS |
| `-z` | Suggest MC-10 mode |

## Subcommands and Shortcodes

Every subcommand accepts a two-letter shortcode:

| Subcommand | Shortcode | Subcommand | Shortcode |
| --- | --- | --- | --- |
| `bulkerase` | `be` | `dumpblock` | `db` |
| `copy` | `cp` | `fstat` | `fs` |
| `dir` | `dr` | `leader` | `le` |
| `dump` | `du` | `list` | `li` |

---

### BULKERASE (`be`) - Create or erase a cassette image

**Syntax:** `bulkerase {[<opts>]} <file> {[<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-s<num>` | Sample rate of WAV file (default: 22050) |
| `-b<num>` | Bits per sample of WAV file: 8, 16, 24, or 32 (default: 16) |
| `-l<num>` | Length of silence to record in WAV file in seconds (default: 0.5) |

---

### DIR (`dr`) - Display the directory

**Syntax:** `dir {[<opts>]} {<dir> [<...>]} {[<opts>]}`

---

### FSTAT (`fs`) - Display file information

**Syntax:** `fstat {[<opts>]} {<file> [<...>]} {[<opts>]}`

---

### DUMP (`du`) - Display contents of a binary file

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

### DUMPBLOCK (`db`) - Dump raw cassette block structures

**Syntax:** `dumpblock {<file> [<...>]}`

**Description:**
Dumps raw cassette blocks (namefile, data, and EOF blocks) from a `.cas`, `.c10`, or `.wav` cassette file with block metadata and checksum verification.

---

### LEADER (`le`) - Write a leader to a cassette file

**Syntax:** `leader {[<opts>]} <file> {[<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-p` | Parity (0 = none [default], 1 = even, 2 = odd) |
| `-s<num>` | Sample rate of WAV file (default: 44100) |
| `-b<num>` | Bits per sample of WAV file (default: 8) |

---

### LIST (`li`) - Display contents of a file

**Syntax:** `list {[<opts>]} {<file> [<...>]} {[<opts>]}`

| Option | Description |
| --- | --- |
| `-t` | perform BASIC token translation |
| `-s` | perform S-Record encoding of binary |

---

### COPY (`cp`) - Copy one or more files to a target image

**Syntax:** `copy {[<opts>]} <srcfile> {[<...>]} <target> {[<opts>]}`

| Option | Description |
| --- | --- |
| `-[0-3]` | file type (0=BASIC, 1=Data, 2=Binary, 3=Source) |
| `-[a\|b]` | data type (a=ASCII, b=Binary) |
| `-[g\|n]` | gap flag (g=Gaps, n=No Gaps) |
| `-d<n>` | load address |
| `-e<n>` | execution address |
| `-l` | perform end of line translation |
| `-t` | perform BASIC entokenization of ASCII text |
| `-k` | perform BASIC detokenization of binary data |
| `-p` | parity (0=none [default], 1=even, 2=odd) |
| `-s` | perform S-Record encode of machine language loadables |
| `-f` | perform S-Record decode of ASCII text file |
| `-c` | perform segment concatenation on machine language loadables |
