# ToolShed Backlog

Working list of known issues, performance opportunities, and memory-churn hotspots found while auditing the codebase. Grouped by theme; each item cites the file/line it's based on. Not exhaustive — this reflects what turned up during a targeted pass, not a full audit.

## Known issues

- **`os9rdump` is built as source but never wired into the `os9` tool.** `os9/os9rdump.c` exists but isn't in the `struct cmdtbl` in `os9/os9_main.c` or in `APPS` in `build/unix/Makefile`. Either add it as a real subcommand or drop the source (per `doc/docs/documentation_review.md`).
- **Four built tools have no user docs**: `dis68`, `lst2cmt`, `makewav`, `tocgen` have no page under `doc/docs/tools/`. `doc/docs/documentation_review.md` already extracted usage text for all four from source — turning that into doc pages is most of the work.
- **`librbf/librbfwrite.c:190`** — modification date/time is not updated on write (`/* 11. TODO - Update modification date/time */`). Files written through `_os9_write` can end up with a stale mtime in the directory entry.
- **`libdecb/libdecbsrec.c:306`** — S-record checksum is read but not verified (`/* TODO check checksum */`). A corrupted `.srec`/`.hex` input can be silently accepted.
- **`os9/os9gen.c:311`** — when regenerating an OS-9 image, bits for replaced files are not deallocated in the bitmap sector (`/* TODO: Deallocate appropriate bits in bitmap sector */`). Regenerated images likely leak allocated-but-unreferenced sectors over repeated `gen` runs.
- **`mamou/pseudo.c:1215`** — explicit known leak: `/* FIXME won't be freed */`.
- **casm parser rough edges**, all self-flagged in source and still open:
  - `casm/src/eval.c:674` — operator priority handling is admittedly incomplete.
  - `casm/src/proc_indexed.c:460` — possible incorrect codegen for a zero offset in indexed addressing.
  - `casm/src/pseudo.c:1505` — `REORG` behavior is marked as unconfirmed correct.
  - `casm/src/proc_logicalmem.c:62` — a compatibility warning path is marked incomplete.
  - `casm/src/parse.c:175` — flagged by the original author as not understood (`/* FIXME - dunno why but FIXME */`); worth a real investigation before it's touched again.
- **Memory-safety convention not yet applied uniformly.** Commit `a91388e` ("Fix Abort trap: 6") established the project's pattern — NULL-initialize handle pointers at declaration, NULL them immediately after `free()`/`term_pd()`/`_os9_close()`, single-exit `goto clean` — and applied it to `librbf/librbfopen.c`, `librbf/librbfdelete.c`, `os9/os9copy.c`, and `os9/os9dcheck.c`. Two structurally identical sibling files still don't follow it:
  - `libdecb/libdecbopen.c` — `term_pd()` (line 557) frees the path struct's contents and the struct itself but never NULLs the caller's `*path` (see call sites at lines 105, 121, 152, 228, 232, 245, 313, 330, 346). Every current caller happens to return immediately afterward, so this isn't a live use-after-free today, but it's inconsistent with the project's own documented invariant and is a landmine for the next person who adds a code path after one of those calls.
  - `libcecb/libcebcopen.c` — same shape: `term_pd()` (line 535) with the identical set of call sites (lines 48, 70, 87, 159, 170, 181, 192, 203, 256, 277, 298, 314, 388, 420) never nulls the caller's pointer either.
  - Worth a follow-up pass applying the same fix used in `libdecbopen.c`/`libcebcopen.c`'s siblings, plus adding the "NULL-close" and "failed-open leaves pointer NULL" tests AGENT_RULES.md already mandates for `librbf`/`libnative` changes.

## Performance improvements

- **`ar2/lz1.c:90`** — the host-side (non-6809) LZW compressor still reads its input one byte at a time via `while (!feof(infile) && (c = getc(infile)) != EOF)`. This is the exact hot path that was just optimized on the 6809 side in commit `4e40152` ("readbuf: replace per-byte getc loop with single fread() call, ~400 cycles saved per buffer fill"). The same fix (buffered `fread()` instead of per-byte `getc()`) applies here too, and since `lz1.c` is the OSK/host reference implementation, worth checking whether its decompression path has the same pattern.
- **`os9/os9rdump.c`** also does per-byte `getc()` reads (lines 221, 262, 276, 398), though since `os9rdump` isn't currently wired into the built `os9` tool (see Known Issues above), this is lower priority until/unless it's actually shipped.

## Reducing memory churn

- **`os9/os9dcheck.c` — `BuildSecondaryAllocationMap()` allocates a fresh sector-sized buffer on every directory/file it visits, rather than reusing one.** This function recurses once per subdirectory and is the core of `os9 dcheck` (the disk-integrity checker), so on a disk with many files it's doing a `malloc`/`free` pair per node instead of a handful total:
  - `dir_fd = malloc(bps)` at line 334 — once per directory.
  - `dEnt = malloc(bps)` at line 398 — once per directory sector scanned.
  - `file_fd = malloc(bps)` at line 456 — once per file entry.
  - `bps` (bytes per sector) is fixed for the life of the check and already known from `os9_path->bps` before recursion starts. A single reusable buffer (or, since `bps` is realistically ≤2048, a fixed-size stack buffer) passed down through the recursive calls would turn this from O(files + directories) allocations into O(1).
- **`decb/decbcopy.c` and `os9/os9copy.c` already avoid this** — the plain (non-translating) copy path in `decbcopy.c` uses a single 32KB stack buffer (`unsigned char chunk_buf[32768]`, line ~270) reused across the whole read/write loop, and `os9copy.c`'s buffer is allocated once outside its loop. No action needed there; noted so the `os9dcheck.c` fix follows the same already-established pattern rather than inventing a new one.
