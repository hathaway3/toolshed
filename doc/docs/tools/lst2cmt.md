# lst2cmt

Convert LWASM listing files into MAME comment files.

## Usage

```bash
lst2cmt {[<opts>]} <srcfile> <destfile> {[<opts>]}
```

## Options

- `-nocrc`: Writes comment lines without a CRC field.
- `-s<system>`: Sets the target system MAME should apply the comments file to (required).
- `-c<cpu>`: Sets the CPU MAME should apply the comments file to (default: `:maincpu`).
- `-nolinenumbers`: Removes line numbers from the listing.
- `-o<offset>`: Specifies an offset for the memory locations to place the comments.

## Description

The `lst2cmt` utility is extremely helpful for debugging assembled code in MAME. By converting an `lwasm` listing file into a format MAME understands, you can view your own source code comments directly within the MAME debugger's disassembly view.
