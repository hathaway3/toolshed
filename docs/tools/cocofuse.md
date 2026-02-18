# CoCoFUSE

CoCoFUSE is a FUSE-compatible file system interface for RBF and Disk BASIC disk images. It allows you to mount a CoCo disk image as a regular directory on your modern operating system (Linux or macOS).

## Installation

### Linux
You will need to have FUSE libraries and header files installed. On Debian-based systems:
```bash
sudo apt-get install libfuse-dev
```

### macOS
CoCoFUSE requires macFUSE. You can install it using Homebrew:
```bash
brew install macfuse
```

## Usage

To mount a disk image, use the following syntax:
```bash
cocofuse <dskimage> <mountpoint> [FUSE options]
```

### Example
```bash
mkdir mnt
cocofuse mydisk.dsk mnt
```

Once mounted, you can navigate the disk image using standard shell commands or your file manager. To unmount:
```bash
# Linux
fusermount -u mnt

# macOS
umount mnt
```
