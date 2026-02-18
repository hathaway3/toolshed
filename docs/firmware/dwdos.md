# DriveWire DOS (DWDOS)

DWDOS is used for booting NitrOS-9 via DriveWire. It works similarly to the `DOS` command in DECB but loads the operating system over the serial link instead of from the floppy boot track.

## Flavours

- **.rom files**: 
    - `_mb_`: For replacing the motherboard ROM.
    - `_dsk_`: For replacing the ROM in a disk controller.
- **.bin files**: DECB binaries for floppy use.
- **.trk files**: Boot track files for track 34 on floppies.

The difference between `_cc1` and `_cc2` flavours is the baud rate.

## Building

```bash
make -C dwdos
```

See the `dwdos/Makefile` for more details.
