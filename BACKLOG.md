# ToolShed Backlog

Working list of known issues, performance opportunities, and memory-churn hotspots found while auditing the codebase. Grouped by theme; each item cites the file/line it's based on. Not exhaustive — this reflects what turned up during a targeted pass, not a full audit.

## Known issues

- [x] **`os9rdump` was not wired into the `os9` tool** — fixed, already in `struct cmdtbl` as of `4df1961`.
- [x] **Four built tools have no user docs** — `dis68`, `lst2cmt`, `makewav`, `tocgen` pages were all added in `6c39116`.
- [x] **`librbf/librbfwrite.c:190` — mtime on write** — fixed, `_os9_write()` now calls `UnixToOS9Time(time(NULL), ...)` on the FD sector before writing it back.
- [x] **`libdecb/libdecbsrec.c:306` — S-record checksum** — fixed, checksum is now verified on S1 records with mismatch causing decode failure.
- [x] **`os9/os9gen.c:311` — bitmap bits not deallocated before re-allocation** — `_os9_delbit()` now frees the boot track clusters before `_os9_allbit()` re-allocates them, preventing leaked sectors on repeated `gen` runs.
- [x] **`mamou/pseudo.c:1215` — explicit known leak** — `as->object_name` is now freed before `strdup()` in both the `-o` option handler and `mamou_main.c`. All assignments use `strdup()` so the pointer is always heap-allocated.
- **casm parser rough edges**, all self-flagged in source and still open:
  - `casm/src/eval.c:674` — operator priority handling is admittedly incomplete.
  - `casm/src/proc_indexed.c:460` — possible incorrect codegen for a zero offset in indexed addressing.
  - `casm/src/pseudo.c:1505` — `REORG` behavior is marked as unconfirmed correct.
  - [x] `casm/src/proc_logicalmem.c:62` — stale FIXME comment removed.
  - [x] `casm/src/parse.c:175` — stale FIXME comment removed.
  - The three remaining items need original-author domain knowledge of 6309 indexed addressing and REORG semantics before they can be acted on.
- [x] **Memory-safety pattern applied to `libdecb/libdecbopen.c` and `libcecb/libcebcopen.c`** — all `term_pd(*path)` error paths in `_decb_open()` and `_cecb_open()` now set `*path = NULL` immediately after freeing, matching the pattern already established in `librbf/librbfopen.c`.
- [x] **`os9/os9rdump.c` — unbounded `getc()` loops & EOF hang** — `showhead()` and `getname()` looped on `while ((c = getc(in)))` without checking for EOF, causing infinite hang on truncated ROF files and overflowing a 10-byte stack buffer. Loops are now bounded to `MAXNAME` and `SYMLEN` with explicit `EOF` checks. Added `feof(in) || ferror(in)` guards in `showglobs()`, `showrefs()`, and `showlcls()` to prevent spinning on EOF-derived 0xFFFF counts. Covered by regression test `test_os9_command_rdump_truncated_file_does_not_hang`.
- [x] **`os9/os9rdump.c:143` — `argv[0][0]` option parsing typo** — fixed typo in option check (`argv[i][0]`) so command-line flags like `-a` are no longer mistaken for non-existent filenames.
- [x] **`librbf/librbfopen.c` — `validate_pathlist()` double-comma truncation** — `validate_pathlist()` previously mutated the path via repeated `strtok()` calls, silently truncating paths containing multiple commas. Now uses `strchr()` to split only at the first comma delimiter and properly handles allocation failures with `EOS_MF`. Covered by `test_os9_command_copy_rejects_double_comma_path`.
- [x] **`librbf/librbfdelete.c:286` — cluster allocation leak on multi-sector clusters** — sector bitmap deallocation in `_os9_delete` calculated cluster offset improperly when `sectors_per_cluster > 1`, causing leaked clusters. Fixed to convert cluster number to physical LSN properly before calling `_os9_delbit()`.
- [x] **`librbf/librbfopen.c:1162` — `_os9_truncate_seg_list` 16-bit underflow** — underflow occurred during segment truncation when `sectors_per_cluster > 1`.
- [x] **`os9/os9attr.c` — multi-character permission reset parsing** — fixed parsing logic for `-npe` / `-npw` flags so public permission unsetting behaves correctly.
- [x] **CLI argument bounds & validation audit** — audited all subcommands across `os9`, `decb`, `cecb`, `makewav`, `tocgen`, `lst2cmt`, and `ar2` for missing argument counts, invalid option handling, and buffer overflows. Added regression coverage in `test_os9_command_line_parameters`.
- [x] **Build & deployment pipeline (`make install` runs tests)** — Root `Makefile` now exposes `make test` and `.PHONY` targets. `build/unix/Makefile` wires `install: test`, ensuring compilation and 100% test pass rate before deploying binaries. `build/unix/unittest/Makefile` properly propagates non-zero exit codes on failure.

## Performance improvements

- [x] **`ar2/lz1.c:90` — per-byte getc() hot path** — `LZ_1()` now reads the entire input into a buffer with a single `fread()` call and iterates in memory, matching the OSK path and the 6809-side optimization from `4e40152`.
- [x] **`os9/os9rdump.c` per-byte getc()** — not a hot path. Retained per-byte I/O for small ROF module headers, but fully bounded all loops with buffer limits and explicit EOF/error checks.

## Reducing memory churn

- [x] **`os9/os9dcheck.c` — malloc churn in `BuildSecondaryAllocationMap()`** — three reusable buffers (`dir_fd`, `dEnt`, `file_fd`) are now allocated once per invocation instead of malloc/free per directory sector and per file entry.
