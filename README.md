# Welcome to ToolShed! 🛠️

[![License: Unlicense](https://img.shields.io/badge/license-Unlicense-blue.svg)](https://unlicense.org) [![Build](https://github.com/nitros9project/toolshed/actions/workflows/build.yml/badge.svg)](https://github.com/nitros9project/toolshed/actions/workflows/build.yml) [![Documentation](https://img.shields.io/badge/docs-mkdocs%20material-blue)](https://nitros9project.github.io/toolshed/)

Version 2.6.1

ToolShed is a comprehensive suite of utilities and source code dedicated to the **Tandy Color Computer (CoCo)** and **Dragon** microcomputers. It provides modern cross-development tools for managing disk images, building firmware, and developing software for these classic systems.

---

## 🚀 Quick Overview

- **🗄️ File System Tools**: Powerful `os9`, `decb`, `cecb`, and `dgndos` utilities for manipulating disk and cassette images.
- **🖥️ DiskShed GUI**: A cross-platform desktop application for browsing and editing OS-9 RBF and Disk BASIC images.
- **💾 Firmware Source**: Complete source code for CoCo and Dragon systems, including custom ROMs like HDB-DOS, DriveWire DOS, and SuperDOS.
- **🏗️ Cross-Development**: Assemblers, disassemblers, and compilers for developing CoCo software on Windows, Linux, and macOS.

---

## 🖥️ DiskShed GUI

DiskShed provides a native wxWidgets interface to ToolShed's OS-9 RBF and Disk
BASIC filesystem libraries. It supports format-aware directory listings,
multiple image windows, import and export, drag and drop, OS-9 directory
navigation, and backup-protected editing operations.

The GUI is optional and uses its own CMake build, so it does not change the
existing command-line build. See the [GUI overview](gui/README.md) and the
[DiskShed build instructions](gui/wx/README.md) for requirements, native
Linux/macOS/Windows release artifacts, and usage.

Prebuilt binaries for macOS, Windows, and Linux are available from
[GitHub Releases](https://github.com/nitros9project/toolshed/releases).
Development builds from the latest commits and pull requests can be downloaded
from the [DiskShed Actions workflow](https://github.com/nitros9project/toolshed/actions/workflows/diskshed.yml);
these workflow artifacts require a GitHub sign-in and expire, so releases are
recommended for general use.

---

## 📚 Documentation

Detailed documentation is available in the `doc/docs/` directory and `doc/archive/ToolShed.md`. If you have MkDocs installed, you can build a local version of our documentation site.

> [!TIP]
> **New to ToolShed?** Start with our [Introduction](doc/docs/introduction.md) or see our [Getting Started guide](doc/docs/getting-started/tutorial.md).

For more detailed command references, please visit our [Wiki](https://github.com/nitros9project/toolshed/wiki).

---

## 🛠️ Installation

### Quick Install (Linux and macOS)

The easiest way to get up and running is with our installation script, which detects dependencies, compiles, runs tests, and installs:

```bash
./install.sh
```

### Manual Build & Installation

Running `make install` automatically compiles all tools, executes the full test suite, and installs the executables and documentation:

#### 🐧 Linux
Install FUSE dependencies, then build and install:
```bash
$ sudo apt-get install libfuse-dev markdown
$ make install
```

#### 🍎 macOS
Install [macFUSE](https://osxfuse.github.io/) or `fuse-t`, then build and install:
```bash
$ brew install macfuse
$ make install
```

#### 🪟 Windows
We recommend using **MSYS2** or **WSL**:
```bash
$ make install
```

> [!NOTE]
> `make install` invokes `./install.sh`, which defaults `PREFIX` to `/usr/local` (using `sudo` automatically if needed) and deploys binaries to `$PREFIX/bin` and HTML documentation to `$PREFIX/share/doc/toolshed`. For a user-local installation without root privileges, pass `PREFIX=$HOME/.local` (e.g. `make install PREFIX=$HOME/.local` or `make -C build/unix install`).

### Running Tests

To compile and run the full test suite (unit tests and CLI integration tests) without installing:

```bash
$ make test
```

To run the unit test suites under AddressSanitizer (`-fsanitize=address,undefined`):

```bash
$ make -C build/unix test-asan
```

---

## 💾 Building Firmware

For building firmware, we strongly recommend installing [LWTOOLS](http://lwtools.projects.l-w.ca/). 

> [!IMPORTANT]
> When building `hdbdos`, make sure to build `cocoroms` first to ensure all required equates are generated.

```bash
$ make -C cocoroms
$ make -C dwdos
$ make -C hdbdos
$ make -C superdos
```

---

## 🤝 Contributing

ToolShed is a community-driven project. We welcome contributions, bug reports, and suggestions!

- [Source](https://github.com/nitros9project/toolshed)
- [Issue Tracker](https://github.com/nitros9project/toolshed/issues)
