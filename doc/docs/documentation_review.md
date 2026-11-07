# Documentation Review Report

## Executive Summary
This report details the findings from a detailed review of the ToolShed project documentation. The review covered the `README.md`, build system (`Makefile`, `install.sh`), and the `docs/` directory.

**Key Findings:**
*   **Build System**: The `README.md` and `Makefile` accurately reflect the build system (`build/unix` and `./install.sh`), noting the deprecation of `build/win32` in favor of `build/unix`.
*   **Core Tools**: Documentation for `os9`, `decb`, `dgndos`, and `cecb` is complete and matches the subcommands, flags, and two-letter shortcodes exposed in the v2.6.1 source code.
*   **Utilities**: Built tools (`ar2`, `cocofuse`, `dis68`, `lst2cmt`, `makewav`, `tocgen`) are documented in `docs/tools/`.
*   **Developer Tools**: `os9/os9rdump.c` exists in the source tree (with bounds and EOF hardening) but is not wired into the default `os9` command table in `os9_main.c`.

## Build & Installation Documentation
*   [x] `README.md` correctly directs users to use `./install.sh` / `make install` or `make -C build/unix`.
*   [x] `install.sh` logic aligns with the `Makefile` targets (`./install.sh $(PREFIX)`).

## Component Documentation

### Tool Verification Table
| Tool | Source Exists | Documentation Exists | Status | Notes |
|------|---------------|----------------------|--------|-------|
| ar2 | Yes | Yes | **OK** | Documented in `tools/utilities.md`. |
| cecb | Yes | Yes | **OK** | Verified all 8 subcommands (`bulkerase`, `copy`, `dir`, `dump`, `dumpblock`, `fstat`, `leader`, `list`) and shortcodes match `cecb_main.c`. |
| cocofuse | Yes | Yes | **OK** | Documented in `tools/cocofuse.md`. |
| decb | Yes | Yes | **OK** | Verified all 15 subcommands (including `binbust`, `fstat`, `info`, `reveal`) and shortcodes match `decb_main.c`. |
| dgndos | Yes | Yes | **OK** | Documented in `tools/dgndos.md`; all 13 subcommands and shortcodes match `dgndos_main.c`. |
| dis68 | Yes | Yes | **OK** | Documented in `tools/dis68.md`. |
| lst2cmt | Yes | Yes | **OK** | Documented in `tools/lst2cmt.md`. |
| makewav | Yes | Yes | **OK** | Documented in `tools/makewav.md`. |
| mamou | Yes | Yes | **OK** | Documented in `tools/mamou.md` (noted as deprecated). |
| os9 | Yes | Yes | **OK** | Verified all 22 subcommands (including `info` and `reveal`) and shortcodes match `os9_main.c`. |
| tocgen | Yes | Yes | **OK** | Documented in `tools/tocgen.md`. |

### `os9` Command Verification
*   **`rdump` Command**: `os9/os9rdump.c` is retained in the source tree (`os9/`), documented in `tools/os9.md` with a note that it is not compiled into the default `os9` command table.
*   **Command Match**: All 22 active `os9` subcommands (`attr`, `cmp`, `copy`, `dcheck`, `del`, `deldir`, `dir`, `dsave`, `dump`, `format`, `free`, `fstat`, `gen`, `id`, `ident`, `info`, `list`, `makdir`, `modbust`, `padrom`, `rename`, `reveal`) match the internal dispatch table in `os9_main.c`.

## Recommendations Status
1.  [x] **Create Documentation**: Created markdown files in `docs/tools/` for `dgndos`, `dis68`, `lst2cmt`, `makewav`, and `tocgen`.
2.  [x] **Clarify `rdump`**: Documented `os9rdump` status in `tools/os9.md`.
3.  [x] **Maintain**: `README.md` points to `doc/docs/` and `doc/archive/ToolShed.md`, and `make install` compiles, tests, and deploys.
