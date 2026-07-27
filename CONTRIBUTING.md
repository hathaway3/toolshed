# Contributing to ToolShed

Welcome! ToolShed is a suite of cross-development tools and firmware for the Tandy Color Computer (CoCo) and Dragon microcomputers. Contributions are welcome.

## Reporting Bugs

Please [open a GitHub Issue](https://github.com/nitros9/toolshed/issues/new?template=bug_report.yml) using the bug report form. Include as much detail as possible about your OS, ToolShed version, and steps to reproduce.

## Feature Requests

Use the [feature request form](https://github.com/nitros9/toolshed/issues/new?template=feature_request.yml) to suggest enhancements.

## Development Setup

1. **Dependencies:**
   - **LWTOOLS** (`lwasm`): required for building firmware (CoCo/Dragon ROMs and DOS). Install from [lwtools.projects.l-w.ca](http://lwtools.projects.l-w.ca/).
   - **libfuse-dev** (Linux) or **macFUSE** (macOS): required for the `cocofuse` FUSE filesystem tool.

2. **Build:**
   ```bash
   make -C build/unix all
   ```

3. **Test:**
   ```bash
   cd build/unix/unittest && make test
   ```

## Coding Style

- **Host-side tools** (root-level `os9`, `decb`, `cecb`, `ar2`, `lib*` directories, etc.): C99 with tabs for indentation (width 8).
- **Firmware** (`hdbdos`, `dwdos`, `cocoroms`, `superdos`): 6809/6309 assembly. `lwasm` is the preferred assembler; the legacy `mamou` assembler works as a fallback.

### Memory Safety (C code)

The C codebase has a history of pointer misuse crashes. Follow these conventions:

- **NULL-initialize** every `os9_path_id` and similar heap-backed handle at declaration.
- **Set pointers to `NULL`** immediately after every `term_pd()`, `_os9_close()`, or `free()` call.
- Use a **single-exit `goto clean;`** pattern; the `clean:` block checks `if (ptr != NULL)` before freeing.
- Never `free()` at multiple return points.

## Pull Request Process

1. Fork the repository.
2. Create a feature branch from `main`.
3. Make your changes and ensure `make test` passes.
4. Submit a pull request using the PR template.

All contributions are released under the same public domain terms as the project.