# Documentation Review Report

## Executive Summary
This report details the findings from a detailed review of the ToolShed project documentation. The review covered the `README.md`, build system (`Makefile`, `install.sh`), and the `docs/` directory.

**Key Findings:**
*   **Build System**: The `README.md` and `docs/guide/building.md` accurately reflect the build system, noting the deprecation of `build/win32` in favor of `build/unix`.
*   **Core Tools**: Documentation for `os9`, `decb`, and `cecb` is accurate and complete, matching the functionality exposed in the source code.
*   **Missing Documentation**: Four built tools (`dis68`, `lst2cmt`, `makewav`, `tocgen`) currently have **no documentation** in `docs/tools/`.
*   **Developer Tools**: The `os9` source contains an `rdump` tool which is not part of the standard build or documentation.

## Build & Installation Documentation
*   [x] `README.md` correctly directs users to use `make -C build/unix` (or the root wrapper).
*   [x] `install.sh` logic aligns with the `Makefile` targets.
*   [x] `docs/guide/building.md` is consistent with `README.md`.

## Component Documentation

### Tool Verification Table
| Tool | Source Exists | Documentation Exists | Status | Notes |
|------|---------------|----------------------|--------|-------|
| ar2 | Yes | Yes | **OK** | |
| cecb | Yes | Yes | **OK** | Verified command list matches source. |
| cocofuse | Yes | Yes | **OK** | |
| decb | Yes | Yes | **OK** | Verified command list matches source. |
| dis68 | Yes | No | **MISSING** | 68k Disassembler. See Usage below. |
| lst2cmt | Yes | No | **MISSING** | LWASM list to MAME comment converter. See Usage below. |
| makewav | Yes | No | **MISSING** | Motorala S-Record to CoCo WAV converter. See Usage below. |
| mamou | Yes | Yes | **OK** | |
| os9 | Yes | Yes | **OK** | Verified command list matches source. |
| tocgen | Yes | No | **MISSING** | Sierra AGI Table of Contents generator. See Usage below. |

### `os9` Command Verification
*   **`rdump` Command**: The file `os9/os9rdump.c` exists but is **not compiled** into the `os9` executable by default, nor is it listed in the `Makefile` `APPS`. It appears to be an unbuilt utility.
*   **Command Match**: All documented `os9` subcommands (attr, copy, dir, etc.) match the internal dispatch table in `os9_main.c`.

## Usage Information for Undocumented Tools

The following information was extracted directly from the source code and can be used to create new documentation pages.

### dis68
**Usage:** `dis68 [options] infile [outfile]`
**Options:**
*   `-m`: file not module format
*   `-o[=]nnnn`: use logical offset of nnnn (hex)
*   `-p name`: use 'name' for parmfile
*   `-s`: create assembler source

### lst2cmt
**Usage:** `lst2cmt {[<opts>]} <srcfile> <destfile> {[<opts>]}`
**Options:**
*   `-nocrc`: Writes comment lines with no CRC field
*   `-s<system>`: Sets the system MAME should apply the comments file to. (Default blank, but required)
*   `-c<cpu>`: Sets the CPU MAME should apply the comments file to. (Default `:maincpu`)
*   `-nolinenumbers`: Remove line numbers.
*   `-o<offset>`: Offset the memory locations to place the comments.

### makewav
**Usage:** `makewav [options] input-file`
**Description:** Converts a Motorola S record file (or raw binary) to CoCo/MC-10 audio WAV file.
**Key Options:**
*   `-l<val>`: Length for silent leader (default 2s)
*   `-s<val>`: Sample rate (default 11250)
*   `-r`: Treat input as raw binary
*   `-c`: Input file has DECB header
*   `-n<string>`: Filename to encode in header
*   `-[0-2]`: File type (0=BASIC, 1=Data, 2=ML)
*   `-[a|b]`: Data type (ASCII/Binary)
*   `-k`: Output in CAS format instead of WAV

### tocgen
**Usage:** `tocgen {[<opts>]} {<infile>} {[<outfile>]} {[<opts>]}`
**Description:** Table of contents generator for Sierra AGI games.
**Options:**
*   `-q`: quiet mode

## Recommendations
1.  **Create Documentation**: Create new markdown files in `docs/tools/` for `dis68`, `lst2cmt`, `makewav`, and `tocgen` using the usage information above.
2.  **Clarify `rdump`**: Decide if `os9rdump` should be added to the build. If not, consider adding a comment in the source code or a note in `os9.md` about its status.
3.  **Maintain**: Continue to keep `README.md` simple and point to `docs/` for detailed usage, as currently implemented.
