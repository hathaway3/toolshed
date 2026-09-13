#!/bin/bash
#
# tests/os9_test.sh - CLI integration test suite for the os9 executable
#
# Tests end-to-end command line workflows of the os9 tool:
# format, copy, dir, list, dump, fstat, free, id, makdir, deldir,
# rename, del, attr, cmp, dcheck, padrom, gen, ident.
#

set -e

# Locate the os9 executable
if [ -n "$OS9" ] && [ -x "$OS9" ]; then
    OS9_BIN="$OS9"
elif [ -x "../os9/os9" ]; then
    OS9_BIN="../os9/os9"
elif [ -x "build/unix/os9/os9" ]; then
    OS9_BIN="build/unix/os9/os9"
elif [ -x "./os9" ]; then
    OS9_BIN="./os9"
elif command -v os9 >/dev/null 2>&1; then
    OS9_BIN="$(command -v os9)"
else
    echo "ERROR: os9 executable not found." >&2
    exit 1
fi

# Convert to absolute path so it works after cd $WORKDIR
OS9_BIN="$(cd "$(dirname "$OS9_BIN")" && pwd)/$(basename "$OS9_BIN")"

# Locate repository root for sample disk reference
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Setup temporary sandbox directory
WORKDIR="$(mktemp -d /tmp/os9_test_XXXXXX)"
trap 'rm -rf "$WORKDIR"' EXIT

cd "$WORKDIR"

PASS=0
FAIL=0

run_test() {
    local desc="$1"
    shift
    if "$@"; then
        PASS=$((PASS + 1))
    else
        echo "FAIL: $desc" >&2
        FAIL=$((FAIL + 1))
    fi
}

# 1. Help & CLI Dispatcher
test_help() {
    "$OS9_BIN" >/dev/null 2>&1
    "$OS9_BIN" -h >/dev/null 2>&1
    "$OS9_BIN" -? >/dev/null 2>&1
    ! "$OS9_BIN" non_existent_command >/dev/null 2>&1
}
run_test "CLI help and unknown command dispatch" test_help

# 2. Format
test_format() {
    "$OS9_BIN" format -e -q -ss -t35 ss.dsk >/dev/null 2>&1
    [ -f ss.dsk ] && [ "$(wc -c < ss.dsk | tr -d ' ')" -eq 161280 ]

    "$OS9_BIN" format -e -q -ds -t35 ds.dsk >/dev/null 2>&1
    [ -f ds.dsk ] && [ "$(wc -c < ds.dsk | tr -d ' ')" -eq 322560 ]

    "$OS9_BIN" format -e -q -l2000 hd.dsk >/dev/null 2>&1
    [ -f hd.dsk ] && [ "$(wc -c < hd.dsk | tr -d ' ')" -eq 512000 ]

    "$OS9_BIN" format -e -q -n"TESTVOL" named.dsk >/dev/null 2>&1
    "$OS9_BIN" id named.dsk | grep -q "TESTVOL"
}
run_test "Format floppy and hard disk geometry variants" test_format

# 3. ID and Free
test_id_free() {
    "$OS9_BIN" id ss.dsk >/dev/null 2>&1
    ! "$OS9_BIN" id nonexistent.dsk >/dev/null 2>&1
    "$OS9_BIN" free ss.dsk | grep -q "Free sectors"
    ! "$OS9_BIN" free nonexistent.dsk >/dev/null 2>&1
}
run_test "Volume ID and free space reporting" test_id_free

# 4. Copy basic round-trip (host -> OS-9 -> host)
test_copy_roundtrip() {
    echo "The quick brown fox jumps over the lazy dog" > original.txt
    "$OS9_BIN" copy original.txt ss.dsk,test.txt >/dev/null 2>&1
    "$OS9_BIN" copy ss.dsk,test.txt roundtrip.txt >/dev/null 2>&1
    cmp -s original.txt roundtrip.txt

    # OS-9 to OS-9 on same disk
    "$OS9_BIN" copy ss.dsk,test.txt ss.dsk,copy.txt >/dev/null 2>&1
    "$OS9_BIN" copy ss.dsk,copy.txt os9_copy.txt >/dev/null 2>&1
    cmp -s original.txt os9_copy.txt
}
run_test "Copy basic round-trip and intra-disk copy" test_copy_roundtrip

# 5. Directory operations (makdir, dir, copy to dir)
test_dirs_and_copy() {
    "$OS9_BIN" makdir ss.dsk,SUBDIR >/dev/null 2>&1
    "$OS9_BIN" makdir ss.dsk,SUBDIR/NESTED >/dev/null 2>&1

    echo "file 1" > f1.txt
    echo "file 2" > f2.txt

    # Copy multiple files to root
    "$OS9_BIN" copy f1.txt f2.txt ss.dsk, >/dev/null 2>&1

    # Copy multiple files to subdirectory
    "$OS9_BIN" copy f1.txt f2.txt ss.dsk,SUBDIR/ >/dev/null 2>&1

    # Copy zero files to directory is a valid no-op (Makefiles with empty variable expansion)
    "$OS9_BIN" copy ss.dsk,SUBDIR/ >/dev/null 2>&1
    "$OS9_BIN" copy -o=0 ss.dsk,SUBDIR/ >/dev/null 2>&1

    # Verify listings
    "$OS9_BIN" dir ss.dsk, | grep -q "f1.txt"
    "$OS9_BIN" dir -e ss.dsk, | grep -q "SUBDIR"
    "$OS9_BIN" dir ss.dsk,SUBDIR | grep -q "f1.txt"

    # Listing nonexistent directory fails
    ! "$OS9_BIN" dir ss.dsk,NONEXISTENT >/dev/null 2>&1
}
run_test "Directory creation and multi-file directory copy" test_dirs_and_copy

# 6. Copy with -r (rewrite)
test_copy_rewrite() {
    echo "version 1" > v1.txt
    echo "version 2" > v2.txt

    "$OS9_BIN" copy v1.txt ss.dsk,rewriteme.txt >/dev/null 2>&1

    # Overwrite without -r must fail
    ! "$OS9_BIN" copy v2.txt ss.dsk,rewriteme.txt >/dev/null 2>&1

    # Overwrite with -r must succeed
    "$OS9_BIN" copy -r v2.txt ss.dsk,rewriteme.txt >/dev/null 2>&1

    "$OS9_BIN" copy ss.dsk,rewriteme.txt out_v2.txt >/dev/null 2>&1
    grep -q "version 2" out_v2.txt
}
run_test "Copy rewrite protection and -r flag" test_copy_rewrite

# 7. Copy with -l (end-of-line translation)
test_copy_eol() {
    printf "Alpha\nBeta\nGamma\n" > unix_lf.txt
    "$OS9_BIN" copy -l unix_lf.txt ss.dsk,eol.txt >/dev/null 2>&1

    # Copy back with translation
    "$OS9_BIN" copy -l ss.dsk,eol.txt back_lf.txt >/dev/null 2>&1
    cmp -s unix_lf.txt back_lf.txt
}
run_test "Copy end-of-line conversion (-l)" test_copy_eol

# 8. Copy buffer sizes and owner options
test_copy_opts() {
    dd if=/dev/zero of=zero.bin bs=1024 count=4 >/dev/null 2>&1
    "$OS9_BIN" copy -b=512 zero.bin ss.dsk,b512.bin >/dev/null 2>&1
    "$OS9_BIN" copy -b=2K -o=42 zero.bin ss.dsk,b2k.bin >/dev/null 2>&1
    "$OS9_BIN" fstat ss.dsk,b2k.bin | grep -q "42"
}
run_test "Copy buffer size and owner flags" test_copy_opts

# 9. Copy disk-full error handling (the regression bug)
test_copy_disk_full() {
    "$OS9_BIN" format -e -q -ss full_test.dsk >/dev/null 2>&1
    dd if=/dev/zero of=huge.bin bs=1024 count=200 >/dev/null 2>&1

    # Attempt to copy file exceeding disk capacity must fail
    ! "$OS9_BIN" copy huge.bin full_test.dsk,huge.bin >/dev/null 2>&1

    # Destination file must not exist
    ! "$OS9_BIN" copy full_test.dsk,huge.bin /dev/null >/dev/null 2>&1

    # Disk must remain structurally consistent
    "$OS9_BIN" dcheck full_test.dsk >/dev/null 2>&1
}
run_test "Copy disk-full error handling, rollback, and cleanup" test_copy_disk_full

# 10. Multi-file copy with partial failure
test_copy_multi_fail() {
    "$OS9_BIN" format -e -q -ss multi_test.dsk >/dev/null 2>&1
    dd if=/dev/zero of=small.bin bs=1024 count=10 >/dev/null 2>&1
    dd if=/dev/zero of=oversized.bin bs=1024 count=200 >/dev/null 2>&1

    ! "$OS9_BIN" copy small.bin oversized.bin multi_test.dsk, >/dev/null 2>&1

    # Small file should exist
    "$OS9_BIN" copy multi_test.dsk,small.bin small_out.bin >/dev/null 2>&1
    [ -f small_out.bin ]

    # Oversized file should not exist
    ! "$OS9_BIN" copy multi_test.dsk,oversized.bin /dev/null >/dev/null 2>&1

    # Disk must be clean
    "$OS9_BIN" dcheck multi_test.dsk >/dev/null 2>&1
}
run_test "Multi-file copy partial failure handling" test_copy_multi_fail

# 11. Copy error cases
test_copy_errors() {
    ! "$OS9_BIN" copy nonexistent_file_xyz.bin ss.dsk,target.bin >/dev/null 2>&1
    ! "$OS9_BIN" copy f1.txt f2.txt ss.dsk,target_file_not_dir.bin >/dev/null 2>&1
    ! "$OS9_BIN" copy f1.txt nonexistent.dsk,f1.bin >/dev/null 2>&1
}
run_test "Copy invalid argument and pathname error cases" test_copy_errors

# 12. List and Dump
test_list_dump() {
    printf "line one\nline two\n" > text.txt
    "$OS9_BIN" copy -l text.txt ss.dsk,list_test.txt >/dev/null 2>&1
    "$OS9_BIN" list ss.dsk,list_test.txt | grep -q "line one"
    ! "$OS9_BIN" list ss.dsk,no_such_file.txt >/dev/null 2>&1

    "$OS9_BIN" dump ss.dsk,list_test.txt >/dev/null 2>&1
    "$OS9_BIN" dump -t -c ss.dsk,list_test.txt >/dev/null 2>&1
    "$OS9_BIN" dump -a ss.dsk,list_test.txt >/dev/null 2>&1
    "$OS9_BIN" dump -e ss.dsk,list_test.txt >/dev/null 2>&1
    ! "$OS9_BIN" dump ss.dsk,no_such_file.txt >/dev/null 2>&1
}
run_test "List and dump viewing commands" test_list_dump

# 13. Fstat
test_fstat() {
    "$OS9_BIN" fstat ss.dsk,test.txt | grep -q "File Information"
    "$OS9_BIN" fstat ss.dsk,SUBDIR | grep -q "File Information"
    ! "$OS9_BIN" fstat ss.dsk,no_such_file.bin >/dev/null 2>&1
}
run_test "Fstat file and directory status inspection" test_fstat

# 14. Rename
test_rename() {
    echo "rename me" > to_rename.txt
    "$OS9_BIN" copy to_rename.txt ss.dsk,before.txt >/dev/null 2>&1
    "$OS9_BIN" rename ss.dsk,before.txt after.txt >/dev/null 2>&1

    # Old name should be gone
    ! "$OS9_BIN" copy ss.dsk,before.txt /dev/null >/dev/null 2>&1

    # New name should exist and match
    "$OS9_BIN" copy ss.dsk,after.txt after.txt >/dev/null 2>&1
    cmp -s to_rename.txt after.txt

    ! "$OS9_BIN" rename ss.dsk,nosuch.txt dummy.txt >/dev/null 2>&1
}
run_test "Rename file on OS-9 disk" test_rename

# 15. Delete and Deldir
test_del() {
    echo "delete me" > del_me.txt
    "$OS9_BIN" copy del_me.txt ss.dsk,todelete.txt >/dev/null 2>&1
    "$OS9_BIN" del ss.dsk,todelete.txt >/dev/null 2>&1

    # Verify file deleted
    ! "$OS9_BIN" copy ss.dsk,todelete.txt /dev/null >/dev/null 2>&1

    # Deleting nonexistent file fails
    ! "$OS9_BIN" del ss.dsk,todelete.txt >/dev/null 2>&1

    # Delete directory
    "$OS9_BIN" deldir -q ss.dsk,SUBDIR/NESTED >/dev/null 2>&1
    ! "$OS9_BIN" dir ss.dsk,SUBDIR/NESTED >/dev/null 2>&1
}
run_test "Delete file and deldir directory removal" test_del

# 16. Attributes
test_attr() {
    echo "attr content" > attr_src.txt
    "$OS9_BIN" copy attr_src.txt ss.dsk,attr.txt >/dev/null 2>&1

    "$OS9_BIN" attr ss.dsk,attr.txt >/dev/null 2>&1
    "$OS9_BIN" attr -q -e -pw ss.dsk,attr.txt >/dev/null 2>&1
    "$OS9_BIN" attr ss.dsk,attr.txt | grep -q "e"
    "$OS9_BIN" attr -q -ne ss.dsk,attr.txt >/dev/null 2>&1
    ! "$OS9_BIN" attr ss.dsk,no_such_file.bin >/dev/null 2>&1
}
run_test "Attribute display and permission modifications" test_attr

# 17. Compare (cmp)
test_cmp() {
    echo "identical content" > c1.txt
    echo "identical content" > c2.txt
    echo "different content" > c3.txt

    "$OS9_BIN" cmp c1.txt c2.txt | grep -q "None"
    "$OS9_BIN" cmp c1.txt c3.txt | grep -q "Bytes different"
    ! "$OS9_BIN" cmp no_such1.txt no_such2.txt >/dev/null 2>&1
}
run_test "File comparison with cmp" test_cmp

# 18. Dcheck
test_dcheck() {
    "$OS9_BIN" dcheck ss.dsk | grep -q "file structure is intact"
    "$OS9_BIN" dcheck -s -b ss.dsk >/dev/null 2>&1
    ! "$OS9_BIN" dcheck nonexistent.dsk >/dev/null 2>&1
}
run_test "Disk structure verification with dcheck" test_dcheck

# 19. Padrom
test_padrom() {
    echo -n "1234" > pad.bin
    "$OS9_BIN" padrom 16 pad.bin >/dev/null 2>&1
    [ "$(wc -c < pad.bin | tr -d ' ')" -eq 16 ]

    "$OS9_BIN" padrom -b -c=65 32 pad.bin >/dev/null 2>&1
    [ "$(wc -c < pad.bin | tr -d ' ')" -eq 32 ]
}
run_test "ROM padding with padrom" test_padrom

# 20. Gen
test_gen() {
    "$OS9_BIN" format -e -q -ss gen.dsk >/dev/null 2>&1
    echo "bootloader code" > boot.bin
    "$OS9_BIN" gen -c gen.dsk -b=boot.bin >/dev/null 2>&1
    "$OS9_BIN" dcheck gen.dsk | grep -q "file structure is intact"
}
run_test "Bootfile linking with gen" test_gen

# 21. Ident (if sample disk available)
test_ident() {
    SAMPLE="$REPO_ROOT/disks/os9l2_d1"
    if [ -f "$SAMPLE" ]; then
        "$OS9_BIN" ident -s "$SAMPLE,OS9Boot" | grep -q "OS9p2"
    fi
}
run_test "OS-9 module header identification with ident" test_ident

# 22. Parameter validation and error codes
test_parameters() {
    ! "$OS9_BIN" -z >/dev/null 2>&1
    ! "$OS9_BIN" format -e -q >/dev/null 2>&1
    ! "$OS9_BIN" format -bogus param.dsk >/dev/null 2>&1
    "$OS9_BIN" format -e -q -t=35 param.dsk >/dev/null 2>&1
    ! "$OS9_BIN" copy >/dev/null 2>&1
    ! "$OS9_BIN" copy single_arg >/dev/null 2>&1
    ! "$OS9_BIN" copy -b file1 file2 >/dev/null 2>&1
    ! "$OS9_BIN" copy -o file1 file2 >/dev/null 2>&1
    ! "$OS9_BIN" copy -bogus file1 file2 >/dev/null 2>&1
    ! "$OS9_BIN" attr >/dev/null 2>&1
    ! "$OS9_BIN" attr -e >/dev/null 2>&1
    ! "$OS9_BIN" attr -bogus param.dsk,test >/dev/null 2>&1
    echo "test" > ptest.txt
    "$OS9_BIN" copy ptest.txt param.dsk,ptest.txt >/dev/null 2>&1
    "$OS9_BIN" attr -q -pe param.dsk,ptest.txt >/dev/null 2>&1
    "$OS9_BIN" attr param.dsk,ptest.txt | grep -q "e"
    "$OS9_BIN" attr -q -npe param.dsk,ptest.txt >/dev/null 2>&1
    ! "$OS9_BIN" dump >/dev/null 2>&1
    ! "$OS9_BIN" dump -t >/dev/null 2>&1
    ! "$OS9_BIN" dump -bogus param.dsk,ptest.txt >/dev/null 2>&1
    ! "$OS9_BIN" rdump >/dev/null 2>&1
    ! "$OS9_BIN" rdump -bogus ptest.txt >/dev/null 2>&1
    ! "$OS9_BIN" gen >/dev/null 2>&1
    ! "$OS9_BIN" gen -b >/dev/null 2>&1
    ! "$OS9_BIN" ident >/dev/null 2>&1
    ! "$OS9_BIN" ident -s >/dev/null 2>&1
    ! "$OS9_BIN" dsave -b >/dev/null 2>&1
}
run_test "CLI parameter error detection and validation" test_parameters

if [ "$FAIL" -gt 0 ]; then
    echo "Tests failed: $FAIL, passed: $PASS" >&2
    exit 1
fi

echo "All $PASS CLI integration tests passed."
exit 0
