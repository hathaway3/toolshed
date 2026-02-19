# C3 Compiler (Microware C Cross-Compiler)

The `c3` directory contains a cross-hosted version of the Microware C compiler for OS-9/6809. This project allows developers to compile C code for the CoCo/Dragon systems on modern host machines.

## Project Structure

- **`comp`**: The actual C compiler components.
- **`compexp`**: Experimental compiler features.
- **`lib`**: The C library source code for the target (OS-9/6809).
- **`include`**: Standard C header files for the compiler.
- **`docs`**: Comprehensive documentation for the C library functions.

---

## C Library Documentation

Documentation for the C library functions is provided in the `c3/docs` directory. Each function has its own text file describing its usage.

### Key Function Categories

- **Standard I/O**: `printf`, `scanf`, `fopen`, `getc`, `putc`, etc.
- **String Manipulation**: `strcpy`, `strlen`, `strcat`, `strcmp`, etc.
- **Memory Management**: `malloc`, `free`, `realloc`, `sbrk`.
- **Operating System Services**: `_os9`, `chain`, `os9fork`, `kill`, `signal`.
- **Filesystem Access**: `open`, `read`, `write`, `close`, `lseek`, `chdir`.
- **System Calls (Direct)**: Direct access to OS-9 system calls via functions like `_gs_size`, `_ss_size`, etc.

For a full list of documented functions, refer to the [C3 Docs Index](../../../c3/docs/index.txt).

---

## Status Note

As mentioned in the [root README.md](../../../README.md), the Microware C compiler for cross-hosted compilation currently "needs work." Use with caution for complex projects.
