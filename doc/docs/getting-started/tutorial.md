# A Tutorial On Disk Images

One of the most frequent questions that come up in dealing with Color Computer disks is "How do I move the data from my old disks over to my PC?" When discussing the moving of files between a NitrOS-9 or Disk BASIC disk, the physical disk media is of no concern to ToolShed -- in fact, this software does not even know how to extract information from a physical diskette. What ToolShed does understand is the logical underpinnings of the RBF and Disk BASIC file systems, and how to properly manipulate files at that level.

The fact that ToolShed is agnostic to physical media begs the question: how does one transfer the contents of a physical disk to a location where ToolShed can work on the image, and vice versa? The answer is dependent upon the host computer.

## Disk Extraction Under Windows/DOS

For the example below, we will assume that the 3.5" 720K floppy drive is `A:`, and the 5.25" 360K floppy drive is `B:`.

The tool required to transfer DSK images to floppies under DOS/Windows is `DSKINI.EXE`.

Once `DSKINI.EXE` has been downloaded and extracted, call up a Command Prompt window and insert a blank 5.25" 360K diskette into drive B. Presuming the image being transfered is `nos96309l2v030200_ds40_1.dsk`, type:

```cmd
DSKINI /T40 /D B: NOS963~1.DSK
```

Once the transfer completes, you should have an exact image of the disk on the floppy.

## Disk Extraction Under Linux

For Linux users, you will need to download the `fdutils` package. Once installed, insert a blank floppy into your drive. It will be either `/dev/fd0` or `/dev/fd1`. For this example, we assume `/dev/fd1`.

You must tell Linux the type of disk in the drive:

```bash
setfdprm /dev/fd1 coco40ds
```

Now you can format and transfer the image:

```bash
fdformat /dev/fd1
dd if=nos96309l2v030200_ds40_1.dsk of=/dev/fd1
```

## Pathname Elements

Since ToolShed runs on various platforms, the `,` (comma) character is used to differentiate between host file system pathnames and paths within the image.

**Example: Copying a file TO an image**
```bash
os9 copy test1 os9.dsk,
```

**Example: Copying a file FROM an image**
```bash
os9 copy os9.dsk,DIR1/DIR2/test2 /
```

Note: The comma is present in both arguments if both are disk image paths.

## Support for HDB-DOS

ToolShed supports specialized path symbols for HDB-DOS hard disk images. The `:` (colon) allows you to select which disk image is active:

```bash
decb copy disk.dsk,NEW.DAT:3 disk.dsk,OLD.DAT:23
```

The `+` specifies an offset to share a hard disk with OS-9:

```bash
decb list -t hd.img,NEW.BAS:3+1348276
```
