# Introduction

ToolShed is a free software product which houses a set of utilities designed to help you create programs and applications for several computing platforms for the Color Computer and Dragon systems running BASIC or NitrOS-9.

ToolShed brings you the ease and speed of developing these programs to your modern personal computer running Windows, macOS, or Linux. The tools in the shed consist of a relocatable macro assembler/linker and intermediate code analyzer, a stand-alone assembler, an OS-9/Disk BASIC file manager, and more.

## History

ToolShed started out life in the mid-to-late 1990s as a 6809 cross-assembler under Linux developed by Alan DeKok and Boisy Pitre. Specifically designed for assembling the source base to NitrOS-9, the assembler was expanded to include support for the 6309. Later, additional tools to read disk images from OS-9 and Disk BASIC images were created and added to the shed.

## Main Components

- **os9**: Manipulate OS-9 formatted disk images.
- **decb**: Manipulate RSDOS (Disk BASIC) formatted disk images.
- **cecb**: Manipulate Color BASIC and Micro Color BASIC cassette files.
- **mamou**: A relocatable macro assembler for the 6809/6309.
- **ar2**: An archiver utility by Carl Kreider.
- **cocofuse**: FUSE based file system for CoCo disk images.
