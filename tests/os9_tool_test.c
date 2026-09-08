/*
 * os9_tool_test.c - Comprehensive OS-9 tool tests
 *
 * Tests the OS-9 API functions for memory safety, proper error handling,
 * and correct behavior. Focuses on areas prone to memory management bugs
 * (double-free, dangling pointers, NULL dereference).
 */

#include "tinytest.h"
#include <toolshed.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_DSK     "os9_tool_test.dsk"
#define TEST_DSK2    "os9_tool_test2.dsk"

/* ---------- helpers -------------------------------------------------- */

static void format_default_disk(const char *name)
{
    unsigned int totalSectors, totalBytes;
    int clusterSize = 0;
    _os9_format((char *)name, 0, 35, 18, 18, 1, 256, &clusterSize,
                "ToolTest", 8, 8, 1, 1, 0, 0, 3, &totalSectors, &totalBytes);
}

/* ---------- null-pointer safety -------------------------------------- */

/*
 * Verify that _os9_close(NULL) does not crash.
 * This is the most basic memory-safety contract.
 */
void test_close_null(void)
{
    error_code ec;
    ec = _os9_close(NULL);
    ASSERT_EQUALS(0, ec);
}

/*
 * Verify that after a failed _os9_open the path pointer stays NULL
 * and a subsequent _os9_close(NULL) is harmless.
 */
void test_open_fail_leaves_null(void)
{
    os9_path_id p = NULL;
    error_code ec;

    ec = _os9_open(&p, "nonexistent.dsk,nosuchfile", FAM_READ);
    ASSERT_EQUALS(EOS_PNNF, ec);
    /* p must still be NULL after a failed open */
    ASSERT_TRUE(p == NULL);

    /* closing NULL must be safe */
    ec = _os9_close(p);
    ASSERT_EQUALS(0, ec);
}

/*
 * Verify that after a failed _os9_create the path pointer stays NULL.
 */
void test_create_fail_leaves_null(void)
{
    os9_path_id p = NULL;
    error_code ec;

    ec = _os9_create(&p, "nonexistent.dsk,nosuchfile", FAM_READ, FAP_READ);
    ASSERT_EQUALS(EOS_PNNF, ec);
    ASSERT_TRUE(p == NULL);

    ec = _os9_close(p);
    ASSERT_EQUALS(0, ec);
}

/* ---------- open / create / close lifecycle -------------------------- */

/*
 * Basic open-close cycle must not leak or double-free.
 */
void test_create_close_cycle(void)
{
    os9_path_id p = NULL;
    error_code ec;

    format_default_disk(TEST_DSK);

    ec = _os9_create(&p, TEST_DSK ",lifecycle.txt",
                     FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
    ASSERT_EQUALS(0, ec);
    ASSERT_TRUE(p != NULL);

    ec = _os9_close(p);
    ASSERT_EQUALS(0, ec);
}

/*
 * Repeated create-close cycles on the same path variable.
 * Catches stale-pointer reuse bugs.
 */
void test_repeated_create_close(void)
{
    os9_path_id p = NULL;
    error_code ec;
    int i;
    char fname[64];

    for (i = 0; i < 10; i++)
    {
        snprintf(fname, sizeof(fname), TEST_DSK ",repeat_%d.txt", i);

        p = NULL;
        ec = _os9_create(&p, fname,
                         FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
        ASSERT_EQUALS(0, ec);
        ASSERT_TRUE(p != NULL);

        ec = _os9_close(p);
        ASSERT_EQUALS(0, ec);
    }
}

/* ---------- write then re-open and read ------------------------------ */

void test_write_read_roundtrip(void)
{
    os9_path_id p = NULL;
    error_code ec;
    char wbuf[] = "Hello, OS-9 round-trip test!";
    char rbuf[64];
    u_int size;

    /* write */
    ec = _os9_create(&p, TEST_DSK ",roundtrip.txt",
                     FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
    ASSERT_EQUALS(0, ec);

    size = (u_int)strlen(wbuf);
    ec = _os9_write(p, wbuf, &size);
    ASSERT_EQUALS(0, ec);

    ec = _os9_close(p);
    ASSERT_EQUALS(0, ec);

    /* read back */
    p = NULL;
    ec = _os9_open(&p, TEST_DSK ",roundtrip.txt", FAM_READ);
    ASSERT_EQUALS(0, ec);
    ASSERT_TRUE(p != NULL);

    memset(rbuf, 0, sizeof(rbuf));
    size = sizeof(rbuf);
    ec = _os9_read(p, rbuf, &size);
    ASSERT_EQUALS(0, ec);
    ASSERT_EQUALS((int)strlen(wbuf), (int)size);
    ASSERT_MEM_EQUALS(wbuf, rbuf, strlen(wbuf));

    ec = _os9_close(p);
    ASSERT_EQUALS(0, ec);
}

/* ---------- directory operations ------------------------------------- */

void test_makdir_and_delete(void)
{
    error_code ec;

    ec = _os9_makdir(TEST_DSK ",TESTDIR");
    ASSERT_EQUALS(0, ec);

    /* duplicate makdir should fail */
    ec = _os9_makdir(TEST_DSK ",TESTDIR");
    ASSERT_EQUALS(EOS_FAE, ec);

    /* nested makdir */
    ec = _os9_makdir(TEST_DSK ",TESTDIR/SUB");
    ASSERT_EQUALS(0, ec);

    /* delete nested first, then parent */
    ec = _os9_delete_directory(TEST_DSK ",TESTDIR");
    ASSERT_EQUALS(0, ec);
}

/* ---------- rename --------------------------------------------------- */

void test_rename(void)
{
    os9_path_id p = NULL;
    error_code ec;

    /* create a file to rename */
    ec = _os9_create(&p, TEST_DSK ",rename_src.txt",
                     FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
    ASSERT_EQUALS(0, ec);
    ec = _os9_close(p);
    ASSERT_EQUALS(0, ec);

    ec = _os9_rename(TEST_DSK ",rename_src.txt", "rename_dst.txt");
    ASSERT_EQUALS(0, ec);

    /* opening old name should fail */
    p = NULL;
    ec = _os9_open(&p, TEST_DSK ",rename_src.txt", FAM_READ);
    ASSERT_EQUALS(EOS_PNNF, ec);
    ASSERT_TRUE(p == NULL);

    /* opening new name should succeed */
    ec = _os9_open(&p, TEST_DSK ",rename_dst.txt", FAM_READ);
    ASSERT_EQUALS(0, ec);
    ec = _os9_close(p);
    ASSERT_EQUALS(0, ec);
}

/* ---------- delete --------------------------------------------------- */

void test_delete_file(void)
{
    os9_path_id p = NULL;
    error_code ec;

    /* create then delete */
    ec = _os9_create(&p, TEST_DSK ",todelete.txt",
                     FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
    ASSERT_EQUALS(0, ec);
    ec = _os9_close(p);
    ASSERT_EQUALS(0, ec);

    ec = _os9_delete(TEST_DSK ",todelete.txt");
    ASSERT_EQUALS(0, ec);

    /* delete again should fail */
    ec = _os9_delete(TEST_DSK ",todelete.txt");
    ASSERT_EQUALS(EOS_PNNF, ec);
}

/* ---------- get-stat calls ------------------------------------------- */

void test_gs_attr(void)
{
    os9_path_id p = NULL;
    error_code ec;
    int perms_out;
    int perms_in = FAP_READ | FAP_WRITE;

    ec = _os9_create(&p, TEST_DSK ",gs_test.txt",
                     FAM_READ | FAM_WRITE, perms_in);
    ASSERT_EQUALS(0, ec);

    ec = _os9_gs_attr(p, &perms_out);
    ASSERT_EQUALS(0, ec);
    ASSERT_EQUALS(perms_in, perms_out);

    ec = _os9_close(p);
    ASSERT_EQUALS(0, ec);
}

void test_gs_size(void)
{
    os9_path_id p = NULL;
    error_code ec;
    u_int file_size;
    char data[] = "0123456789ABCDEF";  /* 16 bytes */
    u_int wsize;

    ec = _os9_create(&p, TEST_DSK ",gs_size.txt",
                     FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
    ASSERT_EQUALS(0, ec);

    wsize = (u_int)strlen(data);
    ec = _os9_write(p, data, &wsize);
    ASSERT_EQUALS(0, ec);

    ec = _os9_gs_size(p, &file_size);
    ASSERT_EQUALS(0, ec);
    ASSERT_EQUALS(16, (int)file_size);

    ec = _os9_close(p);
    ASSERT_EQUALS(0, ec);
}

/* ---------- free-space reporting ------------------------------------- */

void test_free_space(void)
{
    error_code ec;
    char dname[64];
    u_int month, day, year, bps, total_sectors, bytes_free;
    u_int free_sectors, largest_free_block, sectors_per_cluster;
    u_int largest_count, sector_count;

    format_default_disk(TEST_DSK2);

    ec = TSRBFFree(TEST_DSK2, dname, &month, &day, &year, &bps,
                   &total_sectors, &bytes_free, &free_sectors,
                   &largest_free_block, &sectors_per_cluster,
                   &largest_count, &sector_count);
    ASSERT_EQUALS(0, ec);
    ASSERT_TRUE(total_sectors > 0);
    ASSERT_TRUE(free_sectors > 0);
    ASSERT_TRUE(free_sectors <= total_sectors);
}

/* ---------- stress: rapid open-write-close-delete cycle -------------- */

/*
 * This is the single most effective test for catching the Abort trap: 6
 * double-free bug.  It creates and destroys many files in quick succession,
 * exercising every path-descriptor alloc/dealloc path.
 */
void test_stress_open_close(void)
{
    os9_path_id p = NULL;
    error_code ec;
    int i;
    char fname[64];
    char data[] = "stress";
    u_int size;

    for (i = 0; i < 50; i++)
    {
        snprintf(fname, sizeof(fname), TEST_DSK ",stress_%03d.txt", i);
        p = NULL;

        ec = _os9_create(&p, fname,
                         FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
        ASSERT_EQUALS(0, ec);

        size = (u_int)strlen(data);
        ec = _os9_write(p, data, &size);
        ASSERT_EQUALS(0, ec);

        ec = _os9_close(p);
        ASSERT_EQUALS(0, ec);
    }

    /* now delete them all */
    for (i = 0; i < 50; i++)
    {
        snprintf(fname, sizeof(fname), TEST_DSK ",stress_%03d.txt", i);
        ec = _os9_delete(fname);
        ASSERT_EQUALS(0, ec);
    }
}

/* ---------- copy / write out-of-space error handling ----------------- */

/*
 * Test that copying a file to a disk that cannot accommodate it fails with
 * EOS_DF, removes the partial/truncated destination file, and does not leak
 * any clusters in the disk allocation bitmap.
 */
void test_copy_short_space_cleanup(void)
{
    FILE *fp;
    error_code ec;
    os9_path_id p = NULL;
    char dname[64];
    u_int month, day, year, bps, total_sectors, bytes_free;
    u_int free_sectors_before, free_sectors_after;
    u_int largest_free_block, sectors_per_cluster, largest_count, sector_count;
    char buffer[1024];
    int i;

    /* Create a 200KB temporary host file (disk has ~154KB free) */
    fp = fopen("too_big_host.tmp", "wb");
    ASSERT_TRUE(fp != NULL);
    memset(buffer, 0xA5, sizeof(buffer));
    for (i = 0; i < 200; i++)
    {
        fwrite(buffer, 1, sizeof(buffer), fp);
    }
    fclose(fp);

    format_default_disk(TEST_DSK2);

    ec = TSRBFFree(TEST_DSK2, dname, &month, &day, &year, &bps,
                   &total_sectors, &bytes_free, &free_sectors_before,
                   &largest_free_block, &sectors_per_cluster,
                   &largest_count, &sector_count);
    ASSERT_EQUALS(0, ec);

    /* Attempt to copy file exceeding disk space */
    ec = TSCopyFile("too_big_host.tmp", TEST_DSK2 ",too_big.bin", 0, 1, 0, 0, buffer, sizeof(buffer));
    ASSERT_EQUALS(EOS_DF, ec);

    /* Verify destination file was deleted and is not found */
    ec = _os9_open(&p, TEST_DSK2 ",too_big.bin", FAM_READ);
    ASSERT_EQUALS(EOS_PNNF, ec);
    ASSERT_TRUE(p == NULL);

    /* Verify free space is completely preserved (no leaked clusters) */
    ec = TSRBFFree(TEST_DSK2, dname, &month, &day, &year, &bps,
                   &total_sectors, &bytes_free, &free_sectors_after,
                   &largest_free_block, &sectors_per_cluster,
                   &largest_count, &sector_count);
    ASSERT_EQUALS(0, ec);
    ASSERT_EQUALS(free_sectors_before, free_sectors_after);

    remove("too_big_host.tmp");
}

/*
 * Test that _os9_write rolls back uncommitted bitmap cluster allocations
 * when disk space is exhausted, so subsequent deletion cleanly frees everything.
 */
void test_write_disk_full_rollback(void)
{
    error_code ec;
    os9_path_id p = NULL;
    char dname[64];
    u_int month, day, year, bps, total_sectors, bytes_free;
    u_int free_sectors_init, free_sectors_final;
    u_int largest_free_block, sectors_per_cluster, largest_count, sector_count;
    char *big_buf;
    u_int write_size;

    format_default_disk(TEST_DSK2);

    ec = TSRBFFree(TEST_DSK2, dname, &month, &day, &year, &bps,
                   &total_sectors, &bytes_free, &free_sectors_init,
                   &largest_free_block, &sectors_per_cluster,
                   &largest_count, &sector_count);
    ASSERT_EQUALS(0, ec);

    ec = _os9_create(&p, TEST_DSK2 ",fill.bin", FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
    ASSERT_EQUALS(0, ec);
    ASSERT_TRUE(p != NULL);

    write_size = 200000;
    big_buf = (char *)malloc(write_size);
    ASSERT_TRUE(big_buf != NULL);
    memset(big_buf, 0x5A, write_size);

    ec = _os9_write(p, big_buf, &write_size);
    ASSERT_EQUALS(EOS_DF, ec);

    free(big_buf);

    ec = _os9_close(p);
    ASSERT_EQUALS(0, ec);

    /* Deleting the failed file must restore free sectors to initial count */
    ec = _os9_delete(TEST_DSK2 ",fill.bin");
    ASSERT_EQUALS(0, ec);

    ec = TSRBFFree(TEST_DSK2, dname, &month, &day, &year, &bps,
                   &total_sectors, &bytes_free, &free_sectors_final,
                   &largest_free_block, &sectors_per_cluster,
                   &largest_count, &sector_count);
    ASSERT_EQUALS(0, ec);
    ASSERT_EQUALS(free_sectors_init, free_sectors_final);
}

/*
 * Test writing into fragmented free space where contiguous runs are smaller
 * than SAS (Sector Allocation Size). Ensures librbf falls back to available
 * cluster runs rather than returning a premature EOS_DF error.
 */
void test_fragmented_free_space_allocation(void)
{
    error_code ec;
    os9_path_id p = NULL;
    int i;
    char fname[64];
    char chunk[512];
    u_int size;

    memset(chunk, 0x3C, sizeof(chunk));

    format_default_disk(TEST_DSK2);

    /* Create 10 files of 512 bytes (2 clusters each on 1 spc disk) */
    for (i = 0; i < 10; i++)
    {
        snprintf(fname, sizeof(fname), TEST_DSK2 ",frag_%d.bin", i);
        ec = _os9_create(&p, fname, FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
        ASSERT_EQUALS(0, ec);
        size = sizeof(chunk);
        ec = _os9_write(p, chunk, &size);
        ASSERT_EQUALS(0, ec);
        _os9_close(p);
    }

    /* Delete even files (0, 2, 4, 6, 8) to create holes of 2 clusters each */
    for (i = 0; i < 10; i += 2)
    {
        snprintf(fname, sizeof(fname), TEST_DSK2 ",frag_%d.bin", i);
        ec = _os9_delete(fname);
        ASSERT_EQUALS(0, ec);
    }

    /*
     * Now write a file that requires 4 clusters (1024 bytes).
     * Since default SAS is 8, and the free holes are only 2 clusters each,
     * this exercises SAS fallback to allocate multiple smaller segments.
     */
    ec = _os9_create(&p, TEST_DSK2 ",span.bin", FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
    ASSERT_EQUALS(0, ec);
    {
        char bigdata[1024];
        memset(bigdata, 0x7E, sizeof(bigdata));
        size = sizeof(bigdata);
        ec = _os9_write(p, bigdata, &size);
        ASSERT_EQUALS(0, ec);
        ASSERT_EQUALS(sizeof(bigdata), size);
        _os9_close(p);
    }

    /* Read back and verify integrity */
    ec = _os9_open(&p, TEST_DSK2 ",span.bin", FAM_READ);
    ASSERT_EQUALS(0, ec);
    {
        char verify[1024];
        size = sizeof(verify);
        ec = _os9_read(p, verify, &size);
        ASSERT_EQUALS(0, ec);
        ASSERT_EQUALS(sizeof(verify), size);
        for (i = 0; i < (int)sizeof(verify); i++)
        {
            ASSERT_EQUALS(0x7E, (unsigned char)verify[i]);
        }
        _os9_close(p);
    }
}

/*
 * Test copying multiple files sequentially: file A fits on disk, but file B
 * exceeds remaining disk capacity. Ensures file A remains intact and readable,
 * file B is removed without leaving corrupted directory entries, and free space
 * accurately accounts for file A only.
 */
void test_multi_file_copy_partial_failure(void)
{
    FILE *fp;
    error_code ec;
    os9_path_id p = NULL;
    char dname[64];
    u_int month, day, year, bps, total_sectors, bytes_free;
    u_int free_sectors_init, free_sectors_after_a, free_sectors_after_b;
    u_int largest_free_block, sectors_per_cluster, largest_count, sector_count;
    char copy_buf[4096];
    char file_buf[1024];
    int i;

    /* Create file_a (20KB - fits comfortably) and file_b (200KB - overflows remaining space) */
    fp = fopen("file_a.tmp", "wb");
    ASSERT_TRUE(fp != NULL);
    memset(file_buf, 'A', sizeof(file_buf));
    for (i = 0; i < 20; i++)
    {
        fwrite(file_buf, 1, sizeof(file_buf), fp);
    }
    fclose(fp);

    fp = fopen("file_b.tmp", "wb");
    ASSERT_TRUE(fp != NULL);
    memset(file_buf, 'B', sizeof(file_buf));
    for (i = 0; i < 200; i++)
    {
        fwrite(file_buf, 1, sizeof(file_buf), fp);
    }
    fclose(fp);

    format_default_disk(TEST_DSK2);

    ec = TSRBFFree(TEST_DSK2, dname, &month, &day, &year, &bps,
                   &total_sectors, &bytes_free, &free_sectors_init,
                   &largest_free_block, &sectors_per_cluster,
                   &largest_count, &sector_count);
    ASSERT_EQUALS(0, ec);

    /* Copy file_a -> must succeed */
    ec = TSCopyFile("file_a.tmp", TEST_DSK2 ",file_a.bin", 0, 1, 0, 0,
                    copy_buf, sizeof(copy_buf));
    ASSERT_EQUALS(0, ec);

    ec = TSRBFFree(TEST_DSK2, dname, &month, &day, &year, &bps,
                   &total_sectors, &bytes_free, &free_sectors_after_a,
                   &largest_free_block, &sectors_per_cluster,
                   &largest_count, &sector_count);
    ASSERT_EQUALS(0, ec);
    ASSERT_TRUE(free_sectors_after_a < free_sectors_init);

    /* Copy file_b -> must fail with EOS_DF */
    ec = TSCopyFile("file_b.tmp", TEST_DSK2 ",file_b.bin", 0, 1, 0, 0,
                    copy_buf, sizeof(copy_buf));
    ASSERT_EQUALS(EOS_DF, ec);

    /* Verify file_b does NOT exist in destination */
    ec = _os9_open(&p, TEST_DSK2 ",file_b.bin", FAM_READ);
    ASSERT_EQUALS(EOS_PNNF, ec);
    ASSERT_TRUE(p == NULL);

    /* Verify file_a is still completely intact */
    ec = _os9_open(&p, TEST_DSK2 ",file_a.bin", FAM_READ);
    ASSERT_EQUALS(0, ec);
    ASSERT_TRUE(p != NULL);
    {
        u_int fsize = 0;
        ec = _os9_gs_size(p, &fsize);
        ASSERT_EQUALS(0, ec);
        ASSERT_EQUALS(20480, fsize);
        _os9_close(p);
    }

    /* Free space after failed copy must remain unchanged from after file_a was copied */
    ec = TSRBFFree(TEST_DSK2, dname, &month, &day, &year, &bps,
                   &total_sectors, &bytes_free, &free_sectors_after_b,
                   &largest_free_block, &sectors_per_cluster,
                   &largest_count, &sector_count);
    ASSERT_EQUALS(0, ec);
    ASSERT_EQUALS(free_sectors_after_a, free_sectors_after_b);

    remove("file_a.tmp");
    remove("file_b.tmp");
}

/* ---------- main ----------------------------------------------------- */

int main(void)
{
    remove(TEST_DSK);
    remove(TEST_DSK2);

    /* Memory safety fundamentals */
    RUN(test_close_null);
    RUN(test_open_fail_leaves_null);
    RUN(test_create_fail_leaves_null);

    /* Format a disk for the remaining tests */
    format_default_disk(TEST_DSK);

    /* Lifecycle */
    RUN(test_create_close_cycle);
    RUN(test_repeated_create_close);

    /* Data integrity */
    RUN(test_write_read_roundtrip);

    /* Directory operations */
    RUN(test_makdir_and_delete);

    /* Rename */
    RUN(test_rename);

    /* Delete */
    RUN(test_delete_file);

    /* GetStat */
    RUN(test_gs_attr);
    RUN(test_gs_size);

    /* Free space */
    RUN(test_free_space);

    /* Stress test for memory safety */
    RUN(test_stress_open_close);

    /* Out-of-space error handling and cleanup */
    RUN(test_copy_short_space_cleanup);
    RUN(test_write_disk_full_rollback);
    RUN(test_fragmented_free_space_allocation);
    RUN(test_multi_file_copy_partial_failure);

    remove(TEST_DSK);
    remove(TEST_DSK2);

    return TEST_REPORT();
}
