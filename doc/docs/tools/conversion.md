# Conversion Utilities

ToolShed includes several small shell scripts for converting file line endings between different operating systems.

## OS-9 / Unix Conversion

### o2u (OS-9 to Unix)
Converts OS-9 line endings (`\r` or `$0D`) to Unix line endings (`\n` or `$0A`).
**Usage:** `o2u <file1> [<file2> ...]`

### u2o (Unix to OS-9)
Converts Unix line endings (`\n` or `$0A`) to OS-9 line endings (`\r` or `$0D`).
**Usage:** `u2o <file1> [<file2> ...]`

---

## DOS Conversion

### d2o (DOS to OS-9)
Converts DOS line endings (`\r\n` or `$0D0A`) to OS-9 line endings (`\r` or `$0D`) by removing the line feed character.
**Usage:** `d2o <file1> [<file2> ...]`

### d2u (DOS to Unix)
Converts DOS line endings (`\r\n` or `$0D0A`) to Unix line endings (`\n` or `$0A`) by removing the carriage return character.
**Usage:** `d2u <file1> [<file2> ...]`
