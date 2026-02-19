# ToolShed Libraries

ToolShed is built upon a set of internal libraries that provide abstract and concrete access to various CoCo-related disk and cassette formats.

## Architecture Overview

The libraries are designed in a layered fashion:

1.  **High-Level API (`libtoolshed`)**: Provides simplified, high-level functions for common tasks like copying files, renaming, and deleting, regardless of the underlying filesystem.
2.  **Unified Path API (`libcoco`)**: Abstracts over different filesystem types (Native, OS-9, DECB, CECB) using a common `coco_path_id` structure.
3.  **Filesystem-Specific Libraries**:
    *   **`librbf`**: Implementation for OS-9 Random Block File (RBF) systems.
    *   **`libdecb`**: Implementation for Disk BASIC (RSDOS) systems.
    *   **`libcecb`**: Implementation for Cassette BASIC systems.
    *   **`libnative`**: Proxy for the host operating system's native filesystem.

---

## Key Libraries

### libtoolshed
The entry point for most high-level operations.
- **Header**: `toolshed.h`
- **Functions**: `TSCopyFile`, `TSMoveFile`, `TSDelete`, `TSRename`, etc.

### libcoco
Provides the `_coco_*` suite of functions that handle path parsing and dispatching to the correct backend.
- **Header**: `cocopath.h`
- **Functions**: `_coco_open`, `_coco_read`, `_coco_write`, `_coco_seek`, etc.

### librbf (OS-9)
Handles the complex RBF structure, including bitmaps, file descriptors, and multi-sector allocation.
- **Header**: `os9path.h`

### libdecb (Disk BASIC)
Handles RSDOS disk images, FAT (File Allocation Table) management, and directory entries.
- **Header**: `decbpath.h`

### libcecb (Cassette BASIC)
Handles cassette images in various formats (WAV, CAS, C10).
- **Header**: `cecbpath.h`

---

## Data Types & Structures

The libraries use a set of common types defined in `cocotypes.h` to ensure cross-platform compatibility.
- **`error_code`**: Standardized return values.
- **`coco_path_id`**: An opaque pointer to a path structure.
- **`coco_file_stat`**: A unified file status structure.
