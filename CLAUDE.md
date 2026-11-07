# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

ToolShed is a suite of cross-development tools and firmware source for the Tandy Color Computer (CoCo) and Dragon microcomputers. Two very different kinds of code live side by side:

- **Host-side C tools** (root-level dirs like `os9`, `decb`, `dgndos`, `cecb`, `ar2`, `dis68`, `makewav`, `lst2cmt`, `tocgen`, `cocofuse`, and the `lib*` libraries, plus the optional wxWidgets desktop GUI under `gui/wx`): ordinary C99 (and C++17 for `gui/wx`), built and run on Linux/macOS/Windows(MinGW/MSYS2/WSL).
- **Target firmware** (`hdbdos`, `dwdos`, `cocoroms`, `superdos`, `c3`, `casm`, `mamou`): 6809/6309 assembly (and a legacy C compiler toolchain under `c3`) that is cross-assembled to run on the actual CoCo/Dragon hardware or emulators.

Don't assume conventions from one side apply to the other.

User documentation lives under `doc/docs/` (MkDocs site, `tools/` subdirectory has per-tool pages) and `doc/archive/ToolShed.md` (single-page markdown manual). `BACKLOG.md` tracks known issues, performance opportunities, and memory-churn hotspots. `AGENT_RULES.md` has detailed conventions for AI agents working on this repo.

## Build commands

Root `Makefile` delegates to `build/unix` (used even for Windows cross-builds via MinGW — `build/win32` is deprecated) and `./install.sh` for installation.

```bash
make all              # build all host-side tools and libraries
make test             # run full test suite (unit tests + CLI integration tests)
make install          # runs ./install.sh (defaults PREFIX=/usr/local; pass PREFIX=$HOME/.local for user install)
make -C build/unix install  # compile, run tests, and deploy to $HOME/.local (build/unix default)
make clean            # clean host tools + firmware dirs (dwdos, cocoroms, hdbdos, superdos)
make package          # build a release tarball
./install.sh          # detects OS, checks deps (FUSE on Linux, macFUSE/fuse-t on Mac), builds+tests+installs
```

Building a single tool/library directly (each has its own `Makefile` under `build/unix/<name>`, using `rules.mak` for shared `CC`/`AR`/`CFLAGS`):

```bash
cd build/unix/os9 && make        # e.g. rebuild just the `os9` tool
```

`cocofuse` (Linux/macOS FUSE mount) and `unittest` are only built on non-Windows targets (`WIN` unset).

Built binaries land in `build/unix/<name>/<name>` (subdirectory named after the tool).

### Assemblers

- **`lwasm`** (LWTOOLS): preferred assembler for 6809/6309 firmware. Not in this repo — install separately.
- **`mamou`**: deprecated 6809 assembler in this repo (not built by default in `build/unix/Makefile`; build manually via `make -C build/unix/mamou`). Still works as a fallback (`AS="mamou -r -q"`) but `lwasm` is preferred.
- **`casm`**: a separate C-based 6809/6309 cross-assembler in `casm/src` (not built by default in `build/unix/Makefile`).

### Firmware

Requires [LWTOOLS](http://lwtools.projects.l-w.ca/) (`lwasm`) — the deprecated `mamou` assembler still works as a fallback (`AS="mamou -r -q"`) but `lwasm` is preferred for new work.

```bash
make -C cocoroms   # MUST be built first — hdbdos depends on equates it generates
make -C dwdos
make -C hdbdos
make -C superdos
```

## Tests

```bash
make test                                # run from root: builds everything, runs all unit + CLI integration tests
cd build/unix && make test              # same, from build/unix
cd build/unix && make test-asan         # build and run all unit tests with AddressSanitizer + UBSan
cd build/unix/unittest && make test     # unit tests + CLI integration tests
./librbftest                            # run a single suite directly (16 suites in build/unix/unittest:
                                         #   librbftest, libdecbtest, libcecbtest, libtoolshedtest,
                                         #   libnativetest, libmisctest, libsystest, libdiskimagetest,
                                         #   libdragondostest, os9commandtest, decbcommandtest,
                                         #   cecbcommandtest, dgndoscommandtest, infocommandtest,
                                         #   commandaliastest, os9_tool_test)
```

- Test framework is **tinytest** (`unittest/tinytest.h`, strictly header-only) — `ASSERT`, `ASSERT_EQUALS`, `RUN(test_fn)`, `TEST_REPORT()`.
- `build/unix/unittest/Makefile` has a dual `vpath` for both `unittest/` and `tests/`, and adds `-I../../../unittest` so new tests placed in `tests/` can still `#include "tinytest.h"`.
- Follow the pattern in `tests/os9_tool_test.c` / `unittest/librbftest.c`: clean slate (`remove("test.dsk")`) before and after, one `test_*` function per behavior, `RUN()` each in `main()`.

## Architecture: the path abstraction layer

The core design is a filesystem abstraction that lets the same file operations (open/read/write/seek/dir/rename/delete/stat) work across five different image/filesystem types through one dispatching layer:

```
include/cocopath.h   -- coco_path_id: tagged union over the five backends below,
                         _coco_* functions (libcoco/) dispatch by _path_type
                         (NATIVE, OS9, DECB, CECB, DRAGONDOS)
  include/nativepath.h    -> libnative/    (plain host filesystem)
  include/os9path.h       -> librbf/       (OS-9 RBF disk images: LSN0, bitmap, file descriptors/clusters)
  include/decbpath.h      -> libdecb/      (RS-DOS/Disk BASIC images)
  include/dragondospath.h -> libdragondos/ (Dragon DOS disk images)
  include/cecbpath.h      -> libcecb/      (Color BASIC / MC-10 cassette .cas/.wav/.c10)
  include/diskimage.h     -> libdiskimage/ (sector container abstraction under librbf/libdecb/libdragondos:
                                            raw, DMK, JVC, VDK, SDF)
```

The CLI tools (`os9/`, `decb/`, `dgndos/`, `cecb/`) are thin command dispatchers: each has a `<tool>_main.c` with a `struct cmdtbl` mapping subcommand keywords (`attr`, `copy`, `dir`, `format`, `free`, ...) and two-letter shortcodes (`at`, `cp`, `dr`, `fo`, `fr`, ...) to a function implemented in a same-named file in that directory (e.g. `os9copy.c` implements `os9copy`). Each backend library (`libnative`, `librbf`, `libdecb`, `libdragondos`, `libcecb`) mirrors this: one file per operation, named `lib<backend><op>.c`.

`libmisc` holds cross-cutting helpers (CoCo↔native text conversion, endian handling, queue, CRC, path-string parsing, `UnixToCoCoError()` for mapping host `errno` to CoCo error codes). `libcoco` implements the unified `_coco_*` path dispatch and filesystem auto-detection layer. `libsys` provides OS-level portability shims (`strcasecmp`, `strptime`, etc.).

`libtoolshed/toolshed.c` (declared in `include/toolshed.h`, prefixed `TS*`) and its companion modules (`cocodumpcommand.c`, `cocoinfo.c`, `tsdsave.c`) build higher-level operations (copy, rename, pad ROM, attr get/set, free-space reporting, shared `dump`/`info`/`dsave` implementations) on top of `_coco_*`.

`cocofuse` mounts a CoCo/Dragon disk image (OS-9, Disk BASIC, or Dragon DOS) as a real FUSE filesystem using this same `_coco_*` layer, rather than duplicating filesystem logic.

## Memory-safety conventions (librbf / libnative / OS-9 path code)

This code has a history of `Abort trap: 6` crashes from `os9_path_id`/pointer misuse. Conventions used throughout `librbf`, `libnative`, and the OS-9 path handling to prevent that class of bug:

- NULL-initialize every `os9_path_id` (and similar heap-backed handle) at declaration — stack garbage looks like a valid pointer to `free()`.
- Immediately set the pointer to `NULL` after every `term_pd()`, `_os9_close()`, or `free()` call, so a second cleanup on an error path can't double-free.
- Use a single-exit `goto clean;` pattern; the `clean:` block checks `if (ptr != NULL)` before freeing. Don't `free()` at multiple return points.
- Use `DEBUG_PRINT()` (`include/debug.h`) to trace allocation/free around `init_pd`/`term_pd`-style calls when debugging — it's a no-op unless built with `-DDEBUG`.
- If a backtrace under a crash shows `free()`/`term_pd` being handed a small-integer-looking pointer (e.g. `0x6`), it's almost always an error code being mistaken for a pointer — audit for a missing `*path = NULL` after close.

Any change to pointer handling in `librbf`/`libnative` should be covered by: a `_os9_close(NULL)` no-crash test, a test that a failed `_os9_open()` leaves the path pointer `NULL`, and a create/write/close/delete stress loop (~50 iterations) to catch stale-pointer reuse.

## Other conventions

- Every header uses `#ifndef _HEADER_NAME_H` include guards (see existing headers for the exact naming pattern per directory).
- Error codes are the `error_code` type; map host `errno` to CoCo error codes via `UnixToCoCoError()` rather than leaking raw `errno` values into CoCo-facing code.
- Makefiles use `$(CC)`/`$(AS)`/`$(AR)` from `rules.mak`, with `$(CROSS)` prefixing for cross-compilation — don't hardcode toolchain binaries.
- `.clangd` at the repo root only adds `-Iinclude -Dunix -DUNIX -D_FILE_OFFSET_BITS=64`; if clangd reports missing headers but `make` (which uses `-Werror`) succeeds, it's a clangd config gap, not a real bug.
