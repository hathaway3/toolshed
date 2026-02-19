# Building ToolShed 🏗️

Ready to build ToolShed from source? This guide will walk you through the process step-by-step. It's easier than you might think!

## What You'll Need

Before we dive in, make sure you have the following tools ready:
*   **Make**: The standard build tool (required for all platforms).
*   **A C Compiler**: GCC or Clang work perfectly.
*   **FUSE Libraries**: Essential if you want to use `cocofuse`.
*   **LWTOOLS**: Highly recommended if you plan to build firmware.

## 🚀 The Quick & Easy Way (Linux & macOS)

If you're on Linux or macOS, you're in luck! We've included a script that does the heavy lifting for you. It checks for dependencies and handles the build process automatically.

Just run:
```bash
./install.sh
```

## 🛠️ The Manual Way

Prefer to have full control? No problem. Here is how to build manually on your favorite platform.

### Linux

First, let's make sure you have the FUSE dependencies installed.
**Debian/Ubuntu:**
```bash
sudo apt-get install libfuse-dev
```

Once that's sorted, simply build and install:
```bash
make all
make install
```

### macOS

On macOS, we recommend using Homebrew to grab the necessary FUSE libraries.

**Install macFUSE:**
```bash
brew install macfuse
```

Then, run the build commands:
```bash
make all
make install
```

### Windows

For Windows development, we suggest using a Unix-like environment such as **MSYS2**, **MinGW**, or **WSL**.

From your environment's terminal, the standard commands apply:
```bash
make all
make install
```

Happy building!
