# Welcome to ToolShed! 🛠️

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

The easiest way to get up and running is with our installation script:

```bash
./install.sh
```

### Manual Build

#### 🐧 Linux
Install FUSE dependencies:
```bash
$ sudo apt-get install libfuse-dev
$ make all && make install
```

#### 🍎 macOS
Install [macFUSE](https://osxfuse.github.io/):
```bash
$ brew install macfuse
$ make all && make install
```

#### 🪟 Windows
We recommend using **MSYS2** or **WSL**.
```bash
$ make all && make install
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

