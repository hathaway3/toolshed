# ToolShed v2.4

## A Color Computer Cross-Development Toolset

![ToolShed](../cover.jpg)

ToolShed is a comprehensive suite of utilities designed for developers working with the **Tandy Color Computer** and **Dragon** systems. Whether you're building for NitrOS-9 or Disk BASIC, ToolShed provides the libraries and command-line tools you need for robust cross-development on modern systems.

---

### 🚀 Quick Start

If you're new to ToolShed, we recommend starting with these resources:

*   **[Introduction](introduction.md)**: A high-level overview of the project and its components.
*   **[Tutorial on Disk Images](getting-started/tutorial.md)**: Learn how to move data between your PC and CoCo disk images.
*   **[Installation Guide](../#quick-install-linux-and-macos)**: Get up and running on Windows, Linux, or macOS.

---

### 🛠️ Core Tools

Our toolset is organized into several specialized utilities:

*   **Disk & Image Manipulation**:
    *   [os9](tools/os9.md): For OS-9/RBF formatted disk images.
    *   [decb](tools/decb.md): For RSDOS (Disk BASIC) disk images.
    *   [cecb](tools/cecb.md): For cassette images (WAV, CAS, C10).
*   **Archiving & Utilities**:
    *   [ar2](tools/utilities.md#ar2-archiver): Lempel-Zev based archiver.
    *   [cocofuse](tools/cocofuse.md): Mount disk images as local filesystems.
*   **Development**:
    *   [casm](tools/casm.md) & [mamou](tools/mamou.md): Assemblers for 6809/6309.
    *   [c3](tools/c3.md): The C3 C Compiler.

---

### 📚 Technical Reference

*   [Libraries](reference/libraries.md): Explore the internal architecture (libtoolshed, libcoco, etc.).
*   [Firmware & ROMs](reference/firmware.md): Source code for HDB-DOS, DriveWire DOS, and more.
*   [GitHub Repository](https://github.com/nitros9project/toolshed): Contribute to the project!
