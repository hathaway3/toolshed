# ToolShed: Agent Rules & Best Practices 🛠️

This document outlines the mandatory rules and best practices for AI agents working on the ToolShed project. Adhering to these ensures consistency across the codebase, respect for hardware limitations (Tandy CoCo/Dragon), and alignment with the developer's preferred workflows.

---

## 🏎️ Core Workflows

### 1. Mandatory Pre-Flight: Pull & Status
> [!IMPORTANT]
> **ALWAYS** run `git pull` and `git status` before starting any new task. This project is active, and staying in sync is critical to avoid merge conflicts.

### 2. LLM Routing (local-preference.md)
Strictly follow the routing rules defined in `local-preference.md`:
- **Cloud (Gemini 3)**: High-level planning, complex refactoring, and architectural decisions.
- **Local (Gemma 3)**: Writing function bodies, unit tests, and routine boilerplate.
- **Remote Server**: Large context analysis, batch operations, and background tasks.

---

## 📁 Technical Architecture & Conventions

### 1. C-Language Standards (ToolShed)
- **Include Guards**: EVERY header must have a standard `#ifndef HEADER_NAME_H` guard.
- **Memory Safety** (CRITICAL — these rules prevent the `Abort trap: 6` class of bugs):
    - **NULL-Initialize All Pointers**: Every `os9_path_id` (and similar heap-backed pointer) MUST be initialized to `NULL` at declaration. Stack variables have garbage values that look like valid pointers to `free()`.
    - **NULL After Free**: After every `term_pd()`, `_os9_close()`, or `free()` call, immediately set the pointer to `NULL`. This prevents double-free if an error path triggers cleanup again.
    - **Single Exit Pattern**: Use `goto clean;` labels to funnel all return paths through one cleanup block. The cleanup block must check `if (ptr != NULL)` before freeing.
    - **Never `free()` at Multiple Return Points**: Consolidate all resource cleanup into the `clean:` label.
    - **Debug Instrumentation**: Use `DEBUG_PRINT()` from `include/debug.h` to trace allocations and frees. This macro compiles to a no-op unless `-DDEBUG` is passed. Always add `DEBUG_PRINT` around `init_pd`/`term_pd` when debugging memory issues.
- **Error Handling**: Use the `error_code` type consistently. Map system `errno` to CoCo errors using `UnixToCoCoError()`.
- **Modularity**: Keep hardware-specific logic in `libcoco` or similar, and keep filesystem logic in `librbf`/`libdecb`.

### 2. 6809 Assembly Standards (Firmware)
- **Assembler**: Target **LWTOOLS** (`lwasm`) exclusively. 
- **Pragmas**: Use `--pragma=condundefzero,nodollarlocal,noindex0tonone` to maintain compatibility with legacy source while using modern assembly features.
- **Equates**: Centralize hardware and OS-9 equates in `cocoroms/` or similar. Do not redefine common equates inside functional `.asm` files.
- **Commenting**: Document register usage at the start of every subroutine (e.g., `; Input: X=Sector, Output: A=Status`).

### 3. Build & Scripting Standards
- **Makefiles**: Use `$(CC)` and `$(AS)` variables; avoid hardcoding. Support cross-compilation by allowing overrides of these variables.
- **Robust Scripts**: Use `set -e` in all shell scripts. Detect the OS types (Linux, Mac, MinGW) for path-sensitive operations.

---

## 🎨 UI & Aesthetics: "Premium Retro"

Any web-based components (e.g., DriveWire clients, status dashboards) MUST follow the **Premium Retro** design system:
- **CRT Terminal Aesthetic**: Green/Amber phosphor colors, scanline overlays, and monospaced typography.
- **Modern Polish**: Use CSS transitions, subtle phosphor glow, and glassmorphism elements to make the retro feel "premium" and high-end.
- **No Placeholders**: Use `generate_image` for any required visual assets.

---

## 🧪 Testing & Validation

### 1. Centralized Testing
- New tests go in `tests/`. Legacy tests remain in `unittest/` until migrated.
- The build system (`build/unix/unittest/Makefile`) uses dual `vpath` to find sources in both directories.
- Test files must `#include "tinytest.h"` — the tinytest framework (header-only + one `.c` file) lives in `unittest/`.
- The Makefile adds `-I../../../unittest` so tests in `tests/` can resolve `tinytest.h`.

### 2. Test Structure Convention
Follow the pattern established in `os9_tool_test.c` and `librbftest.c`:
```c
#include "tinytest.h"
#include <toolshed.h>

void test_feature_name(void) {
    /* setup, action, assert */
}

int main(void) {
    remove("test.dsk");       /* clean slate */
    RUN(test_feature_name);
    remove("test.dsk");       /* cleanup */
    return TEST_REPORT();
}
```

### 3. Memory Safety Tests (MANDATORY for librbf changes)
Any change to pointer management in `librbf` or `libnative` MUST include:
- **NULL-close test**: `_os9_close(NULL)` must not crash.
- **Failed-open pointer test**: After a failed `_os9_open()`, the path pointer must remain `NULL`.
- **Rapid create-close stress test**: At least 50 iterations of create→write→close→delete to catch stale pointer reuse.

### 4. Stress Testing
For DriveWire or filesystem-related changes, always perform **stress tests** involving random sector read/writes to verify stability and cache integrity.

### 5. Running Tests
```bash
cd build/unix && make          # build everything
cd unittest && make test       # run all unit + integration tests
./librbftest                   # run a single test suite
```

---

## 📜 Documentation
- Update `README.md` if any build steps or dependencies change.
- Document any new CLI flags or configuration options in the `doc/` directory or the GitHub Wiki.

---

## 🔧 IDE & Tooling

### 1. clangd Configuration
The project includes a `.clangd` file at the repo root. This tells the clangd language server where headers are (`-Iinclude`) and what preprocessor defines to use.
- **Paths MUST be relative** — never hardcode absolute paths like `/Users/username/...`.
- If clangd reports "file not found" errors for project headers but the build succeeds with `-Werror`, the issue is the `.clangd` config, not the source code.

### 2. Debugging Memory Issues
When investigating crashes like `Abort trap: 6`:
1. Rebuild with `-DDEBUG` to enable `DEBUG_PRINT` tracing.
2. Run under `lldb` with `--batch -o "run" -o "bt" -o "quit"` to get a backtrace.
3. Check the backtrace for `term_pd` / `free()` — if the pointer value looks like a small integer (e.g., `0x6`, `0xa`), an error code is being treated as a pointer.
4. Audit the function for missing `*path = NULL` after close/term calls.
