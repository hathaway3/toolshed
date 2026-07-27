# ToolShed Backlog

Working list of known issues, performance opportunities, and memory-churn hotspots found while auditing the codebase. Grouped by theme; each item cites the file/line it's based on. Not exhaustive — this reflects what turned up during a targeted pass, not a full audit.

## Known issues

- [x] **`os9rdump` was not wired into the `os9` tool** — fixed, already in `struct cmdtbl` as of `4df1961`.
- **Four built tools have no user docs**: `dis68`, `lst2cmt`, `makewav`, `tocgen` have no page under `doc/docs/tools/`. `doc/docs/documentation_review.md` already extracted usage text for all four from source — turning that into doc pages is most of the work.
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

## Performance improvements

- [x] **`ar2/lz1.c:90` — per-byte getc() hot path** — `LZ_1()` now reads the entire input into a buffer with a single `fread()` call and iterates in memory, matching the OSK path and the 6809-side optimization from `4e40152`.
- [x] **`os9/os9rdump.c` per-byte getc()** — `os9rdump` is now wired in (see above), so `os9rdump.c` per-byte reads at lines 221, 262, 276, 398 are worth fixing. Low priority.

## Reducing memory churn

- [x] **`os9/os9dcheck.c` — malloc churn in `BuildSecondaryAllocationMap()`** — three reusable buffers (`dir_fd`, `dEnt`, `file_fd`) are now allocated once per invocation instead of malloc/free per directory sector and per file entry.
