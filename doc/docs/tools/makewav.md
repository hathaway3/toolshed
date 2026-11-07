# makewav

A Motorola S-Record or raw binary to CoCo/MC-10 audio WAV converter.

## Usage

```bash
makewav [options] input-file
```

## Options

- `-l<val>`: Length for the silent leader in seconds (default: `2`).
- `-s<val>`: Sample rate for the generated WAV file (default: `11250` Hz).
- `-r`: Treat input file as raw binary (default: Motorola S-Record file).
- `-c`: Input file contains DECB binary headers.
- `-n<string>`: Specify the filename to encode in the cassette header (default: input filename).
- `-o<string>`: Output filename for the generated WAV/CAS file (default: `<input>.wav` or `<input>.cas`).
- `-[0-2]`: Set the file type (`0` = BASIC program, `1` = BASIC data, `2` = Machine Language [default]).
- `-[a|b]`: Set the data type (`a` = ASCII, `b` = Binary [default]).
- `-d<val>`: Start/load address (default: `0`, or taken from S-Record/DECB header; accepts `0x` hex).
- `-e<val>`: Execution address (default: `0`, or taken from S-Record/DECB header; accepts `0x` hex).
- `-g<val>`: Group `<val>` data blocks together separated by a pause and new leader.
- `-p<val>`: Pause length in seconds between block groups when using `-g` (supports decimals).
- `-k`: Output in `.cas` format instead of `.wav`.
- `-v`: Print verbose information about the conversion.

## Description

The `makewav` tool is essential for converting your compiled programs into a format that can be loaded onto real hardware via the cassette port. It can process both Motorola S-Record files and raw binary files, adding the necessary sync leaders and headers for the Color Computer and MC-10 to recognize the data.
