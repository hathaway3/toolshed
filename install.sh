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
    *)          OSTYPE="UNKNOWN:${OS}";;
esac

echo "Detected OS: $OSTYPE"

PREFIX="${1:-${PREFIX:-/usr/local}}"
INSTALLDIR="${PREFIX}/bin"
DOCDIR="${PREFIX}/share/doc/toolshed"

SUDO=""
if [ "$(id -u)" -ne 0 ]; then
    if command -v sudo >/dev/null 2>&1; then
        SUDO="sudo"
    fi
fi

can_write_to_dir() {
    local target="$1"
    while [ ! -d "$target" ] && [ "$target" != "/" ] && [ "$target" != "." ]; do
        target="$(dirname "$target")"
    done
    [ -w "$target" ]
}

EXTRA_MAKE_FLAGS=""

install_linux_deps() {
    echo "Checking for build tools and libraries..."
    if [ -f /etc/debian_version ]; then
        DEPS=""
        if ! dpkg -s libfuse-dev >/dev/null 2>&1; then
            DEPS="$DEPS libfuse-dev"
        fi
        if ! dpkg -s build-essential >/dev/null 2>&1; then
            DEPS="$DEPS build-essential"
        fi
        if [ -n "$DEPS" ]; then
            echo "Installing$DEPS..."
            if [ "$(id -u)" -ne 0 ] && [ -z "$SUDO" ]; then
                echo "WARNING: Root privileges needed to install packages, but sudo was not found." >&2
                if ! dpkg -s libfuse-dev >/dev/null 2>&1; then
                    echo "Building ToolShed without FUSE support (cocofuse will be omitted)." >&2
                    EXTRA_MAKE_FLAGS="NO_FUSE=1"
                fi
            else
                if ! ($SUDO apt-get update && $SUDO apt-get install -y $DEPS); then
                    echo "WARNING: Package installation encountered errors." >&2
                    if ! dpkg -s libfuse-dev >/dev/null 2>&1; then
                        echo "Building ToolShed without FUSE support (cocofuse will be omitted)." >&2
                        EXTRA_MAKE_FLAGS="NO_FUSE=1"
                    fi
                fi
            fi
        fi
    elif [ -f /etc/redhat-release ] || [ -f /etc/fedora-release ]; then
        PKG_MGR="yum"
        command -v dnf >/dev/null 2>&1 && PKG_MGR="dnf"
        if ! rpm -q fuse-devel make gcc >/dev/null 2>&1; then
            echo "Installing fuse-devel and build tools..."
            if [ "$(id -u)" -ne 0 ] && [ -z "$SUDO" ]; then
                echo "WARNING: Root privileges needed to install packages, but sudo was not found." >&2
            else
                $SUDO $PKG_MGR install -y fuse-devel make gcc || true
            fi
        fi
    elif [ -f /etc/arch-release ]; then
        if ! pacman -Q fuse2 base-devel >/dev/null 2>&1; then
            echo "Installing fuse2 and base-devel..."
            if [ "$(id -u)" -ne 0 ] && [ -z "$SUDO" ]; then
                echo "WARNING: Root privileges needed to install packages, but sudo was not found." >&2
            else
                $SUDO pacman -Sy --noconfirm fuse2 base-devel || true
            fi
        fi
    elif [ -f /etc/alpine-release ]; then
        if ! apk info -e fuse-dev build-base >/dev/null 2>&1; then
            echo "Installing fuse-dev and build-base..."
            if [ "$(id -u)" -ne 0 ] && [ -z "$SUDO" ]; then
                echo "WARNING: Root privileges needed to install packages, but sudo was not found." >&2
            else
                $SUDO apk add fuse-dev build-base || true
            fi
        fi
    else
        echo "WARNING: Unknown Linux distribution. Please ensure libfuse-dev and build tools are installed."
    fi
}

install_mac_deps() {
    # Check if Homebrew is available or needs to be loaded into environment
    if ! command -v brew > /dev/null 2>&1; then
        if [ -x /opt/homebrew/bin/brew ]; then
            eval "$(/opt/homebrew/bin/brew shellenv)"
        elif [ -x /usr/local/bin/brew ]; then
            eval "$(/usr/local/bin/brew shellenv)"
        fi
    fi

    if ! command -v brew > /dev/null 2>&1; then
        echo "Homebrew not found."
        if [ -d /usr/local/include/fuse ] || [ -d /opt/homebrew/include/fuse ] || [ -d /Library/Filesystems/macfuse.fs ]; then
            echo "Found existing FUSE installation."
        else
            echo "Building ToolShed without FUSE support (cocofuse will be omitted)."
            echo "To enable cocofuse later, install macfuse and rebuild."
            EXTRA_MAKE_FLAGS="NO_FUSE=1"
        fi
        return 0
    fi

    echo "Checking for macfuse..."
    if brew list --cask macfuse > /dev/null 2>&1 || brew list macfuse > /dev/null 2>&1 || [ -d /Library/Filesystems/macfuse.fs ]; then
        echo "macfuse is already installed."
    else
        echo "Attempting to install macfuse..."
        if ! brew install --cask macfuse; then
            echo "WARNING: macfuse installation failed or requires manual approval in System Settings." >&2
            echo "Building ToolShed without FUSE support (cocofuse will be omitted)." >&2
            EXTRA_MAKE_FLAGS="NO_FUSE=1"
        fi
    fi
}

# Main Installation Logic
case "$OSTYPE" in
    Linux|Mac)
        if [ "$OSTYPE" = "Linux" ]; then
            install_linux_deps
        else
            install_mac_deps
        fi

        echo "Building and testing..."
        make -C build/unix test $EXTRA_MAKE_FLAGS

        echo "Installing to $PREFIX..."
        if can_write_to_dir "$INSTALLDIR" && can_write_to_dir "$DOCDIR"; then
            make -C build/unix install PREFIX="$PREFIX" $EXTRA_MAKE_FLAGS
        elif [ -n "$SUDO" ]; then
            echo "Sudo access required to install to $PREFIX"
            $SUDO make -C build/unix install PREFIX="$PREFIX" $EXTRA_MAKE_FLAGS
        else
            echo "Error: Cannot write to $PREFIX and sudo is not available." >&2
            echo "Try running with a user-writable prefix, e.g.: PREFIX=\$HOME/.local ./install.sh" >&2
            exit 1
        fi
        ;;
    MinGW)
        echo "Building for Windows (MinGW)..."
        make -C build/unix install CC=gcc WIN=1 PREFIX="$PREFIX"
        ;;
    *)
        echo "Unsupported OS: $OS"
        exit 1
        ;;
esac

echo "Installation complete!"
