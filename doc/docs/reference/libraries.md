# ToolShed Libraries

ToolShed is built upon a set of internal libraries that provide abstract and concrete access to various CoCo-related disk and cassette formats.

## Architecture Overview

The libraries are designed in five layers:

1.  **High-Level API (`libtoolshed`)**: Provides shared high-level operations for copying files, renaming, deleting, hex/binary dumping (`cocodump`), image inspection (`cocoinfo`), and batch copy script generation (`tsdsave`), regardless of the underlying filesystem.
2.  **Unified Path API (`libcoco`)**: Abstracts over five path backends (Native, OS-9, DECB, CECB, and Dragon DOS) using a common `coco_path_id` descriptor.
3.  **Filesystem-Specific Libraries**:
    *   **`librbf`**: Implementation for OS-9 Random Block File (RBF) filesystems.
    *   **`libdecb`**: Implementation for Disk Extended Color BASIC (RS-DOS) filesystems.
    *   **`libdragondos`**: Implementation for Dragon DOS filesystems.
    *   **`libcecb`**: Implementation for Color BASIC and Micro Color BASIC cassette files.
    *   **`libnative`**: Proxy for the host operating system's native filesystem.
4.  **Sector Container Abstraction (`libdiskimage`)**: Provides uniform logical track/head/sector and LSN access across raw (`.dsk`), `.dmk`, `.jvc`, `.vdk`, and `.sdf` disk image containers.
5.  **Support & Portability (`libmisc`, `libsys`)**: Cross-cutting helpers (EOL conversion, CRC calculation, big-endian/little-endian helpers, queue management, `UnixToCoCoError()`) and platform shims.

---

## Key Libraries

### libtoolshed
The entry point for most high-level operations and shared command implementations.
- **Header**: `toolshed.h`
- **Functions**: `TSCopyFile`, `TSMoveFile`, `TSDelete`, `TSRename`, `TSFree`, `TSPadROM`, `cocodump`, `cocoinfo`, `TSDSave`, etc.

### libcoco
Provides the `_coco_*` suite of functions that handle path parsing, filesystem auto-detection, and dispatching to the correct backend.
- **Header**: `cocopath.h`
- **Functions**: `_coco_open`, `_coco_create`, `_coco_read`, `_coco_write`, `_coco_seek`, `_coco_gs_fd`, `_coco_ss_fd`, etc.

### librbf (OS-9)
Handles the OS-9 RBF structure, including LSN0, allocation bitmaps, file descriptors, and cluster segments.
- **Header**: `os9path.h`

### libdecb (Disk BASIC)
Handles RS-DOS disk images, granule FAT (File Allocation Table) management, directory entries, S-Record conversion, and Color BASIC tokenization.
- **Header**: `decbpath.h`

### libdragondos (Dragon DOS)
Handles Dragon DOS disk images (track 20 superblock, allocation bitmaps, directory entries, continuation blocks, and Dragon BASIC tokenization).
- **Header**: `dragondospath.h`

### libcecb (Cassette BASIC)
Handles cassette files in WAV, CAS, and C10 formats.
- **Header**: `cecbpath.h`

### libdiskimage (Sector Containers)
Handles sector-level container formats (`raw`, `dmk`, `jvc`, `vdk`, `sdf`), geometry translation, and write-protection semantics beneath `librbf`, `libdecb`, and `libdragondos`.
- **Header**: `diskimage.h`

---

## Data Types & Structures

The libraries use a set of common types defined in `cocotypes.h` to ensure cross-platform compatibility.
- **`error_code`**: Standardized return values.
- **`coco_path_id`**: An opaque pointer to a path structure.
- **`coco_file_stat`**: A unified file status structure.
