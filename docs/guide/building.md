# Building ToolShed

This guide provides instructions for building ToolShed on various platforms.

## Prerequisites

- **Make**: Required for all platforms.
- **Compiler**: A C compiler (like GCC or Clang).
- **FUSE Libraries**: Required for `cocofuse`.
- **LWTOOLS**: Highly recommended for building firmware.

## Quick Install (Linux and macOS)

Use the included installation script:
```bash
./install.sh
```

## Manual Build

### Linux
Install FUSE dependencies:
```bash
sudo apt-get install libfuse-dev
```
Then run:
```bash
make all
make install
```

### macOS
Install macFUSE using Homebrew:
```bash
brew install macfuse
```
Then run:
```bash
make all
make install
```

### Windows
Use **MSYS2**, **MinGW**, or **WSL**.
```bash
make all
make install
```
