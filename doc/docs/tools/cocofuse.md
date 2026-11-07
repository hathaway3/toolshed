# cocofuse

`cocofuse` enables mounting CoCo and Dragon disk images (OS-9 RBF, Disk BASIC, and Dragon DOS, across raw, DMK, JVC, VDK, and SDF containers) as local filesystems on Linux and macOS using FUSE.

## Prerequisites

- **Linux**: Requires `libfuse-dev` (and `pkg-config`).
- **macOS**: Requires `macfuse` or `fuse-t` (can be installed via Homebrew).

## Usage

```bash
cocofuse <dskimage> <mountpoint> [FUSE options]
```

To unmount:

```bash
# Linux
fusermount -u <mountpoint>

# macOS
umount <mountpoint>
```

Highly useful for seamless interaction between the host OS and disk images, allowing you to use standard file management tools (like `cp`, `ls`, `rm`) directly on the mounted image.
