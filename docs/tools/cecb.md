# cecb

The `cecb` tool is used to manipulate Color BASIC and Micro Color BASIC formatted cassette images.

## Supported Formats
`cecb` can work with WAV, CAS, and C10 cassette file containers.

## Subcommands

### [BULKERASE](#bulkerase)
Create a cassette image of a given characteristics.

### [DIR](#dir)
Display the directory of a Cassette BASIC image.

### [FSTAT](#fstat)
Display the file descriptor for a file.

### [DUMP](#dump)
Display the contents of a binary file.

### [LIST](#list)
Display the contents of a file.

### [COPY](#copy)
Copy one or more files to a target image.

---

## BULKERASE <a name="bulkerase"></a>

**Syntax:** `bulkerase {[<opts>]} <file> {[<...>]} {[<opts>]}`

**Options:**
- `-s[num]`: Sample rate of WAV file (default: 22050).
- `-b[num]`: Bits per sample of WAV files (default: 8).
- `-l[num]`: Length of silence to record in WAV file (default: 0.5 seconds).
