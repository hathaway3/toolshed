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
| dis68 | Yes | Yes | **OK** | Documented in `tools/dis68.md`. |
| lst2cmt | Yes | Yes | **OK** | Documented in `tools/lst2cmt.md`. |
| makewav | Yes | Yes | **OK** | Documented in `tools/makewav.md`. |
| mamou | Yes | Yes | **OK** | |
| os9 | Yes | Yes | **OK** | Verified command list matches source (including `rdump`). |
| tocgen | Yes | Yes | **OK** | Documented in `tools/tocgen.md`. |

### `os9` Command Verification
*   **`rdump` Command**: `os9/os9rdump.c` is compiled into the `os9` executable by default and documented in `tools/os9.md`.
*   **Command Match**: All documented `os9` subcommands (attr, copy, dir, rdump, etc.) match the internal dispatch table in `os9_main.c`.

## Recommendations Status
1.  [x] **Create Documentation**: Created markdown files in `docs/tools/` for `dis68`, `lst2cmt`, `makewav`, and `tocgen`.
2.  [x] **Clarify `rdump`**: `os9rdump` is integrated into the `os9` tool and documented in `tools/os9.md`.
3.  [x] **Maintain**: `README.md` points to `docs/` and `make install` compiles, tests, and deploys.
