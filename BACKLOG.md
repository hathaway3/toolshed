# ToolShed Backlog

Working list of known issues, performance opportunities, and memory-churn hotspots found while auditing the codebase. Grouped by theme; each item cites the file/line it's based on. Not exhaustive — this reflects what turned up during a targeted pass, not a full audit.

## Known issues

- [x] **`os9rdump` was not wired into the `os9` tool** — wired in `4df1961`, though subsequently omitted from `os9_main.c` / `build/unix/os9/Makefile` when merging upstream v2.6.1 (`os9/os9rdump.c` remains in source with bounds/EOF fixes applied).
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
- [x] **`libcecb/libcecbread.c:104` — `_cecb_readln` uninitialized memory check & delimiter detection** — incremented `current` pointer before checking `*current == 0x0d`, evaluating uninitialized stack memory instead of the byte just read from disk/tape and failing to terminate lines on CR. Fixed to evaluate the read character before incrementing pointer. Covered by `test_cecb_readln`.
- [x] **`libnative/libnativereadln.c:46` — `_native_readln` newline size reporting & empty line truncation** — set `*size = i` instead of `*size = i + 1` upon encountering LF (`0x0A`), omitting the delimiter byte and returning size 0 on empty lines (`\n`). Fixed to return `i + 1`. Covered by `test_native_readln`.
- [x] **`libdecb/libdecbbinconcat.c:75,118` — `_decb_binconcat` 64K boundary truncation** — loop conditions `i < 65535` stopped before processing address 0xFFFF, truncating the last byte of memory in binaries spanning the top of address space. Fixed to `i < 65536`. Covered by `test_decb_binconcat`.
- [x] **`libdecb/libdecbbinconcat.c:192` — `_decb_count_segements` double-increment & buffer overrun** — `else if (buffer[buffer_position++] == POSTAMBLE)` incremented `buffer_position` twice on non-preamble bytes, corrupting postamble detection and segment counts. Fixed to inspect `int amble = buffer[buffer_position++]` with bounds checking. Covered by `test_decb_count_segments`.
- [x] **Build & deployment pipeline (`make install` runs tests)** — Root `Makefile` now exposes `make test` and `.PHONY` targets. `build/unix/Makefile` wires `install: test`, ensuring compilation and 100% test pass rate before deploying binaries. `build/unix/unittest/Makefile` properly propagates non-zero exit codes on failure.

## Performance improvements

- [x] **`ar2/lz1.c:90` — per-byte getc() hot path** — `LZ_1()` now reads the entire input into a buffer with a single `fread()` call and iterates in memory, matching the OSK path and the 6809-side optimization from `4e40152`.
- [x] **`os9/os9rdump.c` per-byte getc()** — not a hot path. Retained per-byte I/O for small ROF module headers, but fully bounded all loops with buffer limits and explicit EOF/error checks.
- [x] **`libtoolshed/toolshed.c` & `coco/cocopadrom.c` — `TSPadROM` chunked writes** — replaced 1-byte write loops with 4096-byte chunked buffer bulk writes, reducing filesystem/disk call overhead by orders of magnitude on large ROMs.
- [x] **`libcoco/libcocoopen.c` — `_coco_open_read_whole_file` pre-allocation & chunking** — queries file size via `_coco_gs_size` to pre-allocate exact buffer upfront, switches resizing from linear +256 byte increments to geometric doubling, and reads in 32KB chunks instead of 256 bytes.
- [x] **`makewav/makewav.c` — `fwrite_repeat_byte` chunked bulk write** — replaced 1-byte `fputc()` loop with a 4096-byte chunked buffer and bulk `fwrite()`.
- [x] **`libcecb/libcecbwav.c` — repeat byte/short bulk writes** — replaced sample-by-sample writes in `_cecb_write_wav_repeat_byte()` and `_cecb_write_wav_repeat_short()` with 4KB chunked bulk `fwrite()`.
- [x] **`libcecb/libcecbbulkerase.c` — `_cecb_bulkerase` bulk writes** — replaced sample-by-sample `fwrite_le_char`/`fwrite_le_short` loop with 4KB bulk `fwrite()`.
- [x] **`librbf/librbfopen.c` — `_os9_close` bulk padding** — replaced 1-byte padding loop with a single bulk `fwrite()`.
- [x] **`tocgen/tocgen_main.c` — `createToc` batched write** — accumulated header and TOC entries into an in-memory buffer, writing it in a single `_coco_write()` call instead of individual byte writes.
- [x] **`libdecb/libdecbbinconcat.c` — `memset` buffer initialization** — replaced 65,535-iteration loop with a single vectorized `memset()`.
- [x] **`os9/os9dump.c` — metadata file size lookup** — queries file size via `_coco_gs_size` and pre-allocates memory directly, avoiding a complete duplicate read pass over the entire file.

## Reducing memory churn & pre-allocating buffers

- [x] **`os9/os9dcheck.c` — malloc churn in `BuildSecondaryAllocationMap()`** — three reusable buffers (`dir_fd`, `dEnt`, `file_fd`) are now allocated once per invocation instead of malloc/free per directory sector and per file entry.
- [x] **`coco/cocodcheck.c` — malloc churn in `BuildSecondaryAllocationMap()`** — ported the reusable buffer pattern (`dir_fd`, `dEnt`, `file_fd`) to eliminate `malloc`/`free` churn inside the inner sector and file entry traversal loops.
- [x] **`libtoolshed/toolshed.c` — `TSCopyFile` translation buffer reuse** — pre-allocates a reusable translation buffer outside the read loop when `eolTranslate == 1`, eliminating per-chunk `malloc`/`free` calls.
- [x] **`libtoolshed/toolshed.c` & `libmisc/libmisccococonv.c` — single-pass EOL translation** — `NativeToCoCo`, `CoCoToNative`, `NativeToDECB`, and `DECBToNative` now translate directly in a single pass without mutating input buffers or performing redundant `memcpy` calls.
- [x] **`libdecb/libdecbtokenize.c` — detokenization pre-allocation & geometric growth** — `_decb_detoken` pre-allocates output buffer to `in_size * 2 + 256` instead of 256 bytes; `_decb_buffer_sprintf` uses geometric doubling instead of linear +256 byte growth.
- [x] **`libcecb/libcecbtokenize.c` — `_cecb_detoken` pre-allocation** — pre-allocates output buffer to `in_size * 2 + 256`.
- [x] **`libdecb/libdecbsrec.c` — `_decb_srec_encode` pre-allocation** — pre-allocates output buffer to `in_size * 3 + 256` to eliminate reallocations during machine-language S-record encoding.
- [x] **`libcecb/libcecbwrite.c` — `_cecb_write_silence` stack buffer** — uses stack buffer for standard CAS/C10 silence lengths (<= 256 bytes) to eliminate heap `calloc`/`free` churn.
- [x] **`libnative/libnativerename.c` — stack buffer & bounds check** — uses stack buffer for normal path lengths to eliminate heap allocations, fixes buffer length calculation, and validates `rename()` return status.
