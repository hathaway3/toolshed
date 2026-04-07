# makewav

A Motorola S-Record or raw binary to CoCo/MC-10 audio WAV converter.

## Usage

```bash
makewav [options] input-file
```

## Options

- `-l<val>`: Length for the silent leader (default: 2 seconds).
- `-s<val>`: Sample rate for the generated WAV file (default: 11250 Hz).
- `-r`: Treat input file as raw binary (no S-Record headers).
- `-c`: Input file contains a DECB header.
- `-n<string>`: Specify the filename to encode in the WAV file's header.
- `-[0-2]`: Set the file type (0 = BASIC, 1 = Data, 2 = ML).
- `-[a|b]`: Set the data type to either ASCII (a) or Binary (b).
- `-k`: Output in CAS format instead of WAV.

## Description

The `makewav` tool is essential for converting your compiled programs into a format that can be loaded onto real hardware via the cassette port. It can process both Motorola S-Record files and raw binary files, adding the necessary sync leaders and headers for the Color Computer and MC-10 to recognize the data.
