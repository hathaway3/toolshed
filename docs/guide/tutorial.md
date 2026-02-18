# Disk Image Tutorial

This tutorial explains how to move data between physical Color Computer disks and your PC.

## Background
ToolShed does not know how to extract information directly from physical diskettes. It understands the logical structure of RBF and Disk BASIC file systems. You need separate tools to create disk images (`.dsk`) from physical media.

## Disk Extraction

### Windows/DOS
Use `DSKINI.EXE` to transfer DSK images to floppies.

```cmd
DSKINI /T40 /D B: MYDISK.DSK
```

### Linux
Use the `fdutils` package.
```bash
# Set floppy parameters
setfdprm /dev/fd1 coco40ds
# Format the disk
fdformat /dev/fd1
# Transfer the image
dd if=mydisk.dsk of=/dev/fd1
```

## Pathname Elements
ToolShed uses the comma (`,`) as a delineator between the host path and the path within the disk image.

**Example: Copying from host to image**
```bash
os9 copy test1 os9.dsk,
```
The comma at the end tells `os9` to copy `test1` to the root of `os9.dsk`. Without the comma, it would overwrite the entire `os9.dsk` file with `test1`.

**Example: Copying from image to host**
```bash
os9 copy os9.dsk,DIR1/DIR2/test2 /
```

## HDB-DOS Support
The colon (`:`) allows you to select which disk image is active in a multi-disk container.
```bash
decb copy disk.dsk,NEW.DAT:3 disk.dsk,OLD.DAT:23
```
This copies `NEW.DAT` from the 4th disk image to `OLD.DAT` on the 24th disk image.
