# Welcome to ToolShed! 🛠️

ToolShed is your one-stop shop for tools and source code dedicated to the Tandy Color Computer and Dragon microcomputers. Whether you're a seasoned developer or just getting started, you'll find a cornucopia of useful utilities here.

## What's Inside?

In this repository, you'll find:
- **File System Tools**: `os9` and `decb` utilities for easily copying files between your host system and disk images.
- **System ROMs**: Source code for CoCo and Dragon system ROMs.
- **Custom ROMs**: Source code for popular custom ROMs like HDB-DOS, DriveWire DOS, and SuperDOS.
- **Compilers**: Source code for the Microware C compiler (currently a work in progress).
- **Cross-Development**: Assemblers and other tools to help you build software from Windows, Linux, and macOS.

> [!NOTE]
> **A Note on Assemblers**: While we include the classic `mamou` 6809 cross-assembler for historical preservation, we highly recommend using William Astle's excellent **LWTOOLS**. It includes the modern `lwasm` assembler and `lwlink` linker. [You can download the latest source here.](http://lwtools.projects.l-w.ca)

For more detailed documentation on specific commands, please visit our [Wiki](https://github.com/nitros9project/toolshed/wiki).

## 🚀 Quick Install (Linux and macOS)

The easiest way to get up and running on Linux or macOS is to use our handy installation script. It will automatically check for dependencies (like FUSE) and build everything for you.

Simply run this command from the root of the ToolShed directory:
```bash
./install.sh
```

Prefer to build manually? No problem! Check out the platform-specific instructions below.

## 🪟 Building on Windows

For Windows users, we recommend using [MingW32 or MingW64](http://mingw.org/), [MSYS2](http://msys2.github.io/), or the [WSL subsystem for Windows 10+](https://en.wikipedia.org/wiki/Windows_Subsystem_for_Linux).

The easiest way to get MingW is by using the installer [found here](http://mingw.org/wiki/Getting_Started).

Once you're in your MingW environment, ensure you have `make` installed. The simplest option is usually:
```
$ mingw-get install mingw-make
```

Then, navigate to the toolshed directory and run:
```
$ make all
$ make install
```
> [!TIP]
> The root `Makefile` handles everything for you by delegating to the appropriate build systems. You can also run `make clean` to tidy up all generated files.

## 🐧 Building on Linux

To build `cocofuse`, you'll need the FUSE libraries and headers. On Debian-based systems (like Ubuntu), you can install them with:
```
$ sudo apt-get install libfuse-dev
```

Then, just run:
```
$ make all
$ make install
```

## 🍎 Building on macOS

To build `cocofuse` on macOS, you'll also need FUSE libraries. We recommend using [Homebrew](https://brew.sh) to manage this.

First, install Homebrew if you haven't already. Then, install `macfuse`:

```
brew install macfuse
```

If you have an older installation of `osxfuse`, you'll need to upgrade:

```
brew uninstall osxfuse
brew install macfuse
```

Finally, enter the toolshed directory and build:
```
$ make all
$ make install
```

## 💾 Building Firmware (HDB-DOS, DriveWire DOS, CoCo ROMs, SuperDOS)

For building firmware, we strongly recommend installing [LWTOOLS](http://lwtools.projects.l-w.ca/). You'll also need the `makewav` tool from this repository (ToolShed) to generating WAV files.

> [!IMPORTANT]
> When building `hdbdos`, make sure to build `cocoroms` first, as it depends on equates generated there.

To build all the default firmware flavors, run:
```
$ make -C cocoroms
$ make -C dwdos
$ make -C hdbdos
$ make -C superdos
```

If you need to use the legacy `mamou` assembler, you can do so like this (though your mileage may vary):
```
$ make -C dwdos AS="mamou -r -q"
$ make -C hdbdos AS="mamou -r -q"
```

*Note: SuperDOS currently builds with `mamou` by default.*
