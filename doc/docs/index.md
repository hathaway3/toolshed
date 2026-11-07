# ToolShed v2.6.1

## A Color Computer & Dragon Cross-Development Toolset

![ToolShed](../cover.jpg)

ToolShed is a comprehensive suite of utilities designed for developers working with the **Tandy Color Computer** and **Dragon** systems. Whether you're building for NitrOS-9, Disk BASIC, or Dragon DOS, ToolShed provides the libraries, command-line tools, and desktop GUI you need for robust cross-development on modern systems.

---

### 🚀 Quick Start

If you're new to ToolShed, we recommend starting with these resources:

*   **[Introduction](introduction.md)**: A high-level overview of the project and its components.
*   **[Tutorial on Disk Images](getting-started/tutorial.md)**: Learn how to move data between your PC and CoCo/Dragon disk images.
*   **[Installation Guide](https://github.com/nitros9project/toolshed#installation)**: Get up and running on Windows, Linux, or macOS.

---

### 🛠️ Core Tools

Our toolset is organized into several specialized utilities:

*   **Disk & Image Manipulation**:
    *   [os9](tools/os9.md): For OS-9/RBF formatted disk images.
    *   [decb](tools/decb.md): For RS-DOS (Disk BASIC) disk images.
    *   [dgndos](tools/dgndos.md): For Dragon DOS disk images.
    *   [cecb](tools/cecb.md): For cassette images (WAV, CAS, C10).
    *   [makewav](tools/makewav.md): Create audio WAV files from binaries.
    *   **DiskShed GUI** (`gui/wx`): Cross-platform desktop application for browsing and editing OS-9 RBF and Disk BASIC images.
*   **Archiving & Utilities**:
    *   [ar2](tools/utilities.md#ar2): Lempel-Ziv based archiver.
    *   [cocofuse](tools/cocofuse.md): Mount disk images as local filesystems.
    *   [tocgen](tools/tocgen.md): Table of contents generator for the CoCo SDC ROM launcher.
*   **Development & Disassembly**:
    *   [casm](tools/casm.md) & [mamou](tools/mamou.md): Assemblers for 6809/6309 (`lwasm` from LWTOOLS is recommended for new work).
    *   [dis68](tools/dis68.md): A disassembler for 68000 (OS-9/68K) modules.
    *   [c3](tools/c3.md): The C3 C Compiler.
    *   [lst2cmt](tools/lst2cmt.md): Convert LWASM listings to MAME comments.

---

### 📚 Technical Reference

*   [Libraries](reference/libraries.md): Explore the internal architecture (`libtoolshed`, `libcoco`, `libdiskimage`, `librbf`, `libdecb`, `libdragondos`, `libcecb`).
*   [Firmware & ROMs](reference/firmware.md): Source code for HDB-DOS, DriveWire DOS, SuperDOS, and CoCo/Dragon ROMs.
*   [GitHub Repository](https://github.com/nitros9project/toolshed): Contribute to the project!
