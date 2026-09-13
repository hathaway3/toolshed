# Welcome to ToolShed! 🛠️

[![License: Unlicense](https://img.shields.io/badge/license-Unlicense-blue.svg)](https://unlicense.org) [![Build](https://github.com/nitros9project/toolshed/actions/workflows/build.yml/badge.svg)](https://github.com/nitros9project/toolshed/actions/workflows/build.yml) [![Documentation](https://img.shields.io/badge/docs-mkdocs%20material-blue)](https://nitros9project.github.io/toolshed/)

ToolShed is a comprehensive suite of utilities and source code dedicated to the **Tandy Color Computer (CoCo)** and **Dragon** microcomputers. It provides modern cross-development tools for managing disk images, building firmware, and developing software for these classic systems.

---

## 🚀 Quick Overview

- **🗄️ File System Tools**: Powerful `os9` and `decb` utilities for manipulating disk and cassette images.
- **💾 Firmware Source**: Complete source code for CoCo and Dragon systems, including custom ROMs like HDB-DOS and DriveWire DOS.
- **🏗️ Cross-Development**: Assemblers, disassemblers, and compilers for developing CoCo software on Windows, Linux, and macOS.

---

## 📚 Documentation

Detailed documentation is available in the `doc/docs/` directory. If you have MkDocs installed, you can build a beautiful local version of our documentation site.

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
$ sudo apt-get install libfuse-dev
$ make install
```

#### 🍎 macOS
Install [macFUSE](https://osxfuse.github.io/), then build and install:
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
> By default, `make install` deploys binaries to `$HOME/.local/bin` and HTML documentation to `$HOME/.local/share/doc/toolshed`. To install system-wide, provide `PREFIX=/usr/local` (e.g. `sudo make install PREFIX=/usr/local`).

### Running Tests

To compile and run the full test suite (unit tests and CLI integration tests) without installing:

```bash
$ make test
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

