#!/bin/bash
# One-line install script for ToolShed

set -e

# Detect OS
OS="$(uname -s)"
case "${OS}" in
    Linux*)     OSTYPE=Linux;;
    Darwin*)    OSTYPE=Mac;;
    MINGW*)     OSTYPE=MinGW;;
    MSYS*)      OSTYPE=MinGW;;
    *)          OSTYPE="UNKNOWN:${OS}"
esac

echo "Detected OS: $OSTYPE"

install_linux_deps() {
    echo "Checking for FUSE libraries..."
    if [ -f /etc/debian_version ]; then
        if ! dpkg -s libfuse-dev >/dev/null 2>&1; then
            echo "Installing libfuse-dev..."
            sudo apt-get update && sudo apt-get install -y libfuse-dev build-essential
        fi
    elif [ -f /etc/redhat-release ]; then
        if ! rpm -q fuse-devel >/dev/null 2>&1; then
            echo "Installing fuse-devel..."
            sudo yum install -y fuse-devel make gcc
        fi
    else
        echo "WARNING: Unknown Linux distribution. Please ensure libfuse-dev and build tools are installed."
    fi
}

install_mac_deps() {
    echo "Checking for Homebrew..."
    if ! command -v brew &> /dev/null; then
        echo "Homebrew not found. Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi

    echo "Checking for macfuse..."
    if ! brew list macfuse &> /dev/null; then
        echo "Installing macfuse..."
        brew install macfuse
    fi
}

# Main Installation Logic
case "$OSTYPE" in
    Linux)
        install_linux_deps
        echo "Building and installing..."
        # Running make install which might require sudo for /usr/local/bin
        if [ -w /usr/local/bin ]; then
             make -C build/unix install
        else
             echo "Sudo access required to install to /usr/local/bin"
             sudo make -C build/unix install
        fi
        ;;
    Mac)
        install_mac_deps
        echo "Building and installing..."
        if [ -w /usr/local/bin ]; then
             make -C build/unix install
        else
             echo "Sudo access required to install to /usr/local/bin"
             sudo make -C build/unix install
        fi
        ;;
    MinGW)
        echo "Building for Windows (MinGW)..."
        # Assuming mingw environment is already set up as per README recommendations for Windows users usually downloading pre-setup environments
        # but we can try to run the install
        make -C build/unix install CC=gcc
        ;;
    *)
        echo "Unsupported OS: $OS"
        exit 1
        ;;
esac

echo "Installation complete!"
