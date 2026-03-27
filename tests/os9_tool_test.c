/*
 * os9_tool_test.c - Comprehensive OS-9 tool tests
 *
 * Tests the OS-9 API functions for memory safety, proper error handling,
 * and correct behavior. Focuses on areas prone to memory management bugs
 * (double-free, dangling pointers, NULL dereference).
 */

#include "tinytest.h"
#include <toolshed.h>

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

    remove(TEST_DSK);
    remove(TEST_DSK2);

    return TEST_REPORT();
}
