/*
 * ALWAYS BE TESTING!!!
 *
 * ALWAYS BE WRITING MORE TESTS!!!
 *
 * THIS WORK IS **NEVER** DONE!!!
 *
 * Comprehensive tests for the os9 CLI executable.
 */

#include "tinytest.h"
#include <toolshed.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utime.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include "imagedamage.h"

/* Enough for the 40 track images of the gap tests */
#define GAP_IMAGE_MAX	(40 * 18 * 256)

static const char *get_os9_bin(void)
{
	if (access("../os9/os9", X_OK) == 0) return "../os9/os9";
	if (access("build/unix/os9/os9", X_OK) == 0) return "build/unix/os9/os9";
	return "os9";
}

static const char *get_sample_disk(void)
{
	if (access("disks/os9l2_d1", R_OK) == 0) return "disks/os9l2_d1";
	if (access("../../../disks/os9l2_d1", R_OK) == 0) return "../../../disks/os9l2_d1";
	if (access("../../disks/os9l2_d1", R_OK) == 0) return "../../disks/os9l2_d1";
	return NULL;
}

static void cleanup_test_files(void)
{
	remove("test.dsk");
	remove("test_cli.dsk");
	remove("test_cli2.dsk");
	remove("test_fmt_ss.dsk");
	remove("test_fmt_ds.dsk");
	remove("test_fmt_name.dsk");
	remove("test_fmt_hd.dsk");
	remove("test_fmt_bad.dsk");
	remove("cli_big.tmp");
	remove("cli_small.tmp");
	remove("cli_src.tmp");
	remove("cli_dst.tmp");
	remove("cli_os9_2.tmp");
	remove("cli_a.tmp");
	remove("cli_b.tmp");
	remove("cli_v1.tmp");
	remove("cli_v2.tmp");
	remove("cli_out.tmp");
	remove("cli_unix.tmp");
	remove("cli_back.tmp");
	remove("cli_buf.tmp");
	remove("cli_pad.tmp");
	remove("cli_boot.tmp");
	remove("cli_text.tmp");
	remove("cli_cmp1.tmp");
	remove("cli_cmp2.tmp");
	remove("cli_cmp3.tmp");
	remove("test_clust2.dsk");
	remove("test_clust4.dsk");
	remove("cli_cl_small.tmp");
	remove("cli_cl_med.tmp");
	remove("cli_cl_large.tmp");
}

/* ---------- 1. Help & CLI Dispatcher ---------- */

void test_os9_command_help_and_unknown(void)
{
	char cmd[256];
	int status;

	/* os9 without arguments prints help and exits 0 */
	snprintf(cmd, sizeof(cmd), "%s > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* os9 -h prints help and exits 0 */
	snprintf(cmd, sizeof(cmd), "%s -h > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* os9 -? prints help and exits 0 */
	snprintf(cmd, sizeof(cmd), "%s -? > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Unknown command must fail with non-zero exit status */
	snprintf(cmd, sizeof(cmd), "%s nonexistent_subcmd_xyz > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);
}

/* ---------- 2. Format ---------- */

void test_os9_command_format(void)
{
	error_code ec;
	native_path_id nativepath = NULL;
	u_int size;
	char cmd[256];

	snprintf(cmd, sizeof(cmd), "%s format -e test.dsk -l65000 > /dev/null 2>&1", get_os9_bin());
	ec = system(cmd);
	ASSERT_EQUALS(0, ec);

	ec = _native_open(&nativepath, "test.dsk", FAM_READ);
	ASSERT_EQUALS(0, ec);

	ec = _native_gs_size(nativepath, &size);
	ASSERT_EQUALS(0, ec);
	ASSERT_EQUALS(65000*256, size);

	ec = _native_close(nativepath);
	nativepath = NULL;
	ASSERT_EQUALS(0, ec);

	ec = _os9_makdir("test.dsk,/CMDS");
	ASSERT_EQUALS(0, ec);
}

void test_os9_command_format_variants(void)
{
	char cmd[256];
	int status;
	native_path_id np = NULL;
	u_int size;

	/* Floppy single-sided */
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss -t35 test_fmt_ss.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);
	_native_open(&np, "test_fmt_ss.dsk", FAM_READ);
	_native_gs_size(np, &size);
	_native_close(np);
	np = NULL;
	ASSERT_EQUALS(161280, size);

	/* Floppy double-sided */
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ds -t35 test_fmt_ds.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);
	_native_open(&np, "test_fmt_ds.dsk", FAM_READ);
	_native_gs_size(np, &size);
	_native_close(np);
	np = NULL;
	ASSERT_EQUALS(322560, size);

	/* Named disk */
	snprintf(cmd, sizeof(cmd), "%s format -e -q -n\"NAMEDISK\" test_fmt_name.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Hard drive with logical sectors */
	snprintf(cmd, sizeof(cmd), "%s format -e -q -l2000 test_fmt_hd.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);
	_native_open(&np, "test_fmt_hd.dsk", FAM_READ);
	_native_gs_size(np, &size);
	_native_close(np);
	np = NULL;
	ASSERT_EQUALS(512000, size);

	/* Invalid format option must fail */
	snprintf(cmd, sizeof(cmd), "%s format -bogus_opt test_fmt_bad.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	remove("test_fmt_ss.dsk");
	remove("test_fmt_ds.dsk");
	remove("test_fmt_name.dsk");
	remove("test_fmt_hd.dsk");
	remove("test_fmt_bad.dsk");
}

/* ---------- 3. Copy (Comprehensive) ---------- */

void test_os9_command_copy_basic_roundtrip(void)
{
	char cmd[256];
	int status;
	FILE *fp;
	char src_buf[1024];
	char dst_buf[1024];
	size_t read_bytes;
	int i;

	/* Create 154KB floppy */
	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Create 1024-byte host file */
	for (i = 0; i < 1024; i++)
	{
		src_buf[i] = (char)(i & 0xFF);
	}
	fp = fopen("cli_src.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite(src_buf, 1, sizeof(src_buf), fp);
	fclose(fp);

	/* 1. Host to OS-9 copy */
	snprintf(cmd, sizeof(cmd), "%s copy cli_src.tmp test_cli.dsk,file1.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* 2. OS-9 to Host copy */
	snprintf(cmd, sizeof(cmd), "%s copy test_cli.dsk,file1.bin cli_dst.tmp > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Verify host-copied content matches byte-for-byte */
	fp = fopen("cli_dst.tmp", "rb");
	ASSERT_TRUE(fp != NULL);
	read_bytes = fread(dst_buf, 1, sizeof(dst_buf), fp);
	fclose(fp);
	ASSERT_EQUALS(sizeof(src_buf), read_bytes);
	ASSERT_MEM_EQUALS(src_buf, dst_buf, sizeof(src_buf));

	/* 3. OS-9 to OS-9 copy (same disk) */
	snprintf(cmd, sizeof(cmd), "%s copy test_cli.dsk,file1.bin test_cli.dsk,file2.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Copy file2.bin back to host and verify */
	snprintf(cmd, sizeof(cmd), "%s copy test_cli.dsk,file2.bin cli_os9_2.tmp > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	fp = fopen("cli_os9_2.tmp", "rb");
	ASSERT_TRUE(fp != NULL);
	memset(dst_buf, 0, sizeof(dst_buf));
	read_bytes = fread(dst_buf, 1, sizeof(dst_buf), fp);
	fclose(fp);
	ASSERT_EQUALS(sizeof(src_buf), read_bytes);
	ASSERT_MEM_EQUALS(src_buf, dst_buf, sizeof(src_buf));

	remove("cli_src.tmp");
	remove("cli_dst.tmp");
	remove("cli_os9_2.tmp");
	remove("test_cli.dsk");
}

void test_os9_command_copy_to_directory(void)
{
	char cmd[256];
	int status;
	os9_path_id p = NULL;
	error_code ec;
	FILE *fp;

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Create directory on OS-9 disk */
	snprintf(cmd, sizeof(cmd), "%s makdir test_cli.dsk,SUBDIR > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Create two host files */
	fp = fopen("cli_a.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("hello a", 1, 7, fp);
	fclose(fp);

	fp = fopen("cli_b.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("hello b", 1, 7, fp);
	fclose(fp);

	/* Copy multiple files into disk root directory */
	snprintf(cmd, sizeof(cmd), "%s copy cli_a.tmp cli_b.tmp test_cli.dsk, > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	ec = _os9_open(&p, "test_cli.dsk,cli_a.tmp", FAM_READ);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE(p != NULL);
	_os9_close(p);
	p = NULL;

	ec = _os9_open(&p, "test_cli.dsk,cli_b.tmp", FAM_READ);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE(p != NULL);
	_os9_close(p);
	p = NULL;

	/* Copy multiple files into subdirectory */
	snprintf(cmd, sizeof(cmd), "%s copy cli_a.tmp cli_b.tmp test_cli.dsk,SUBDIR/ > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	ec = _os9_open(&p, "test_cli.dsk,SUBDIR/cli_a.tmp", FAM_READ);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE(p != NULL);
	_os9_close(p);
	p = NULL;

	ec = _os9_open(&p, "test_cli.dsk,SUBDIR/cli_b.tmp", FAM_READ);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE(p != NULL);
	_os9_close(p);
	p = NULL;

	remove("cli_a.tmp");
	remove("cli_b.tmp");
	remove("test_cli.dsk");
}

void test_os9_command_copy_rewrite_option(void)
{
	char cmd[256];
	int status;
	FILE *fp;
	char read_buf[32];

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	fp = fopen("cli_v1.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("version 1", 1, 9, fp);
	fclose(fp);

	fp = fopen("cli_v2.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("version 2", 1, 9, fp);
	fclose(fp);

	/* Copy initial version */
	snprintf(cmd, sizeof(cmd), "%s copy cli_v1.tmp test_cli.dsk,target.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Copying second version to same name without -r MUST fail */
	snprintf(cmd, sizeof(cmd), "%s copy cli_v2.tmp test_cli.dsk,target.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Copying second version with -r MUST succeed */
	snprintf(cmd, sizeof(cmd), "%s copy -r cli_v2.tmp test_cli.dsk,target.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Verify target.txt has version 2 content */
	snprintf(cmd, sizeof(cmd), "%s copy test_cli.dsk,target.txt cli_out.tmp > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	fp = fopen("cli_out.tmp", "rb");
	ASSERT_TRUE(fp != NULL);
	memset(read_buf, 0, sizeof(read_buf));
	fread(read_buf, 1, sizeof(read_buf) - 1, fp);
	fclose(fp);
	ASSERT_STRING_EQUALS("version 2", read_buf);

	remove("cli_v1.tmp");
	remove("cli_v2.tmp");
	remove("cli_out.tmp");
	remove("test_cli.dsk");
}

void test_os9_command_copy_eol_translation(void)
{
	char cmd[256];
	int status;
	os9_path_id p = NULL;
	error_code ec;
	FILE *fp;
	char raw[64];
	u_int rsize;
	char back_buf[64];

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Write Unix LF text file */
	fp = fopen("cli_unix.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fputs("Line1\nLine2\nLine3\n", fp);
	fclose(fp);

	/* Copy to OS-9 with -l (LF -> CR translation) */
	snprintf(cmd, sizeof(cmd), "%s copy -l cli_unix.tmp test_cli.dsk,text.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Verify raw OS-9 file contains CR (0x0D) line endings */
	ec = _os9_open(&p, "test_cli.dsk,text.txt", FAM_READ);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE(p != NULL);

	memset(raw, 0, sizeof(raw));
	rsize = sizeof(raw) - 1;
	ec = _os9_read(p, raw, &rsize);
	ASSERT_EQUALS(0, ec);
	_os9_close(p);
	p = NULL;

	ASSERT_STRING_EQUALS("Line1\rLine2\rLine3\r", raw);

	/* Copy from OS-9 to host with -l (CR -> LF translation) */
	snprintf(cmd, sizeof(cmd), "%s copy -l test_cli.dsk,text.txt cli_back.tmp > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	fp = fopen("cli_back.tmp", "rb");
	ASSERT_TRUE(fp != NULL);
	memset(back_buf, 0, sizeof(back_buf));
	fread(back_buf, 1, sizeof(back_buf) - 1, fp);
	fclose(fp);

	ASSERT_STRING_EQUALS("Line1\nLine2\nLine3\n", back_buf);

	remove("cli_unix.tmp");
	remove("cli_back.tmp");
	remove("test_cli.dsk");
}

void test_os9_command_copy_buffer_and_owner(void)
{
	char cmd[256];
	int status;
	os9_path_id p = NULL;
	error_code ec;
	FILE *fp;
	char buffer[1024];
	fd_stats fd;
	int fd_size;
	int i;

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Create 4KB file */
	fp = fopen("cli_buf.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	memset(buffer, 0x42, sizeof(buffer));
	for (i = 0; i < 4; i++)
	{
		fwrite(buffer, 1, sizeof(buffer), fp);
	}
	fclose(fp);

	/* Copy with -b=512 */
	snprintf(cmd, sizeof(cmd), "%s copy -b=512 cli_buf.tmp test_cli.dsk,buf512.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Copy with -b=2K -o=42 */
	snprintf(cmd, sizeof(cmd), "%s copy -b=2K -o=42 cli_buf.tmp test_cli.dsk,buf2k.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Check owner of buf2k.bin is 42 */
	ec = _os9_open(&p, "test_cli.dsk,buf2k.bin", FAM_READ);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE(p != NULL);

	fd_size = sizeof(fd);
	ec = _os9_gs_fd(p, fd_size, &fd);
	ASSERT_EQUALS(0, ec);
	ASSERT_EQUALS(42, fd.fd_own[1]);

	_os9_close(p);
	p = NULL;

	remove("cli_buf.tmp");
	remove("test_cli.dsk");
}

void test_os9_command_copy_error_cases(void)
{
	char cmd[256];
	int status;
	FILE *fp;

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Copy nonexistent source file must fail */
	snprintf(cmd, sizeof(cmd), "%s copy nonexistent_file_xyz.tmp test_cli.dsk,file.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Create two dummy host files */
	fp = fopen("cli_a.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("a", 1, 1, fp);
	fclose(fp);

	fp = fopen("cli_b.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("b", 1, 1, fp);
	fclose(fp);

	/* Copying multiple sources to a non-directory target must fail */
	snprintf(cmd, sizeof(cmd), "%s copy cli_a.tmp cli_b.tmp test_cli.dsk,not_a_dir.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Copying to nonexistent disk image must fail */
	snprintf(cmd, sizeof(cmd), "%s copy cli_a.tmp nonexistent_disk.dsk,file.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	remove("cli_a.tmp");
	remove("cli_b.tmp");
	remove("test_cli.dsk");
}

void test_os9_command_copy_disk_full(void)
{
	int status;
	os9_path_id p = NULL;
	error_code ec;
	FILE *fp;
	char buffer[1024];
	char cmd[256];
	int i;

	/* Create floppy disk image (154KB free) */
	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Create 200KB host file */
	fp = fopen("cli_big.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	memset(buffer, 0xEE, sizeof(buffer));
	for (i = 0; i < 200; i++)
	{
		fwrite(buffer, 1, sizeof(buffer), fp);
	}
	fclose(fp);

	/* Attempt copy via os9 command - must fail with non-zero exit status */
	snprintf(cmd, sizeof(cmd), "%s copy cli_big.tmp test_cli.dsk,cli_big.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Verify file was not left in directory */
	ec = _os9_open(&p, "test_cli.dsk,cli_big.bin", FAM_READ);
	ASSERT_EQUALS(EOS_PNNF, ec);
	ASSERT_TRUE(p == NULL);

	/* Verify dcheck confirms 0 errors / 0 orphaned clusters */
	snprintf(cmd, sizeof(cmd), "%s dcheck test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	remove("cli_big.tmp");
	remove("test_cli.dsk");
}

void test_os9_command_copy_multi_file_failure(void)
{
	int status;
	os9_path_id p = NULL;
	error_code ec;
	FILE *fp;
	char buffer[1024];
	char cmd[256];
	int i;

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Create a 10KB small file and a 200KB oversized file */
	fp = fopen("cli_small.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	memset(buffer, 'S', sizeof(buffer));
	for (i = 0; i < 10; i++)
	{
		fwrite(buffer, 1, sizeof(buffer), fp);
	}
	fclose(fp);

	fp = fopen("cli_big.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	memset(buffer, 'B', sizeof(buffer));
	for (i = 0; i < 200; i++)
	{
		fwrite(buffer, 1, sizeof(buffer), fp);
	}
	fclose(fp);

	/* Copy both files to disk in one command; multi-file copy must return non-zero */
	snprintf(cmd, sizeof(cmd), "%s copy cli_small.tmp cli_big.tmp test_cli.dsk, > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Small file should have been copied successfully before disk filled */
	ec = _os9_open(&p, "test_cli.dsk,cli_small.tmp", FAM_READ);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE(p != NULL);
	_os9_close(p);
	p = NULL;

	/* Big file should NOT be left corrupted on disk */
	ec = _os9_open(&p, "test_cli.dsk,cli_big.tmp", FAM_READ);
	ASSERT_EQUALS(EOS_PNNF, ec);
	ASSERT_TRUE(p == NULL);

	/* Verify dcheck passes with 0 errors */
	snprintf(cmd, sizeof(cmd), "%s dcheck test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	remove("cli_small.tmp");
	remove("cli_big.tmp");
	remove("test_cli.dsk");
}

/* ---------- 4. Dir ---------- */

void test_os9_command_dir(void)
{
	char cmd[256];
	int status;
	FILE *fp;

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	fp = fopen("cli_a.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("data", 1, 4, fp);
	fclose(fp);

	snprintf(cmd, sizeof(cmd), "%s copy cli_a.tmp test_cli.dsk,file_a.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s makdir test_cli.dsk,MYDIR > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Plain dir */
	snprintf(cmd, sizeof(cmd), "%s dir test_cli.dsk, > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Extended dir */
	snprintf(cmd, sizeof(cmd), "%s dir -e test_cli.dsk, > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Show all files */
	snprintf(cmd, sizeof(cmd), "%s dir -a test_cli.dsk, > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Subdir listing */
	snprintf(cmd, sizeof(cmd), "%s dir test_cli.dsk,MYDIR > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Nonexistent dir must fail */
	snprintf(cmd, sizeof(cmd), "%s dir test_cli.dsk,NOSUCHDIR > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	remove("cli_a.tmp");
	remove("test_cli.dsk");
}

/* ---------- 5. ID & Free ---------- */

void test_os9_command_id_and_free(void)
{
	char cmd[256];
	int status;

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* id on valid disk */
	snprintf(cmd, sizeof(cmd), "%s id test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* id on nonexistent disk */
	snprintf(cmd, sizeof(cmd), "%s id nonexistent.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* free on valid disk */
	snprintf(cmd, sizeof(cmd), "%s free test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* free on nonexistent disk */
	snprintf(cmd, sizeof(cmd), "%s free nonexistent.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	remove("test_cli.dsk");
}

/* ---------- 6. List & Dump ---------- */

void test_os9_command_list_and_dump(void)
{
	char cmd[256];
	int status;
	FILE *fp;

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	fp = fopen("cli_text.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fputs("hello world\nsecond line\n", fp);
	fclose(fp);

	snprintf(cmd, sizeof(cmd), "%s copy -l cli_text.tmp test_cli.dsk,text.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* list text file */
	snprintf(cmd, sizeof(cmd), "%s list test_cli.dsk,text.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* list nonexistent file */
	snprintf(cmd, sizeof(cmd), "%s list test_cli.dsk,nonexistent.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* dump default */
	snprintf(cmd, sizeof(cmd), "%s dump test_cli.dsk,text.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* dump options */
	snprintf(cmd, sizeof(cmd), "%s dump -t -c test_cli.dsk,text.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s dump -a test_cli.dsk,text.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s dump -e test_cli.dsk,text.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* dump nonexistent file */
	snprintf(cmd, sizeof(cmd), "%s dump test_cli.dsk,nonexistent.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	remove("cli_text.tmp");
	remove("test_cli.dsk");
}

/* ---------- 7. Fstat ---------- */

void test_os9_command_fstat(void)
{
	char cmd[256];
	int status;
	FILE *fp;

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	fp = fopen("cli_a.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("fstat test", 1, 10, fp);
	fclose(fp);

	snprintf(cmd, sizeof(cmd), "%s copy cli_a.tmp test_cli.dsk,stat.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* fstat on existing file */
	snprintf(cmd, sizeof(cmd), "%s fstat test_cli.dsk,stat.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* fstat on directory */
	snprintf(cmd, sizeof(cmd), "%s makdir test_cli.dsk,STATDIR > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s fstat test_cli.dsk,STATDIR > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* fstat on nonexistent file must fail with non-zero */
	snprintf(cmd, sizeof(cmd), "%s fstat test_cli.dsk,nonexistent.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	remove("cli_a.tmp");
	remove("test_cli.dsk");
}

/* ---------- 8. Makdir & Deldir ---------- */

void test_os9_command_makdir_and_deldir(void)
{
	char cmd[256];
	int status;
	os9_path_id p = NULL;
	error_code ec;

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Create directory */
	snprintf(cmd, sizeof(cmd), "%s makdir test_cli.dsk,DIR1 > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Create nested directory */
	snprintf(cmd, sizeof(cmd), "%s makdir test_cli.dsk,DIR1/SUB1 > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Creating duplicate directory reports warning but exits 0 (PR #70),
	 * while creating a directory over an existing file must fail. */
	snprintf(cmd, sizeof(cmd), "%s makdir test_cli.dsk,DIR1 > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	ec = _os9_create(&p, "test_cli.dsk,EXISTING_FILE", FAM_WRITE, FAP_READ | FAP_WRITE);
	ASSERT_EQUALS(0, ec);
	_os9_close(p);
	p = NULL;
	snprintf(cmd, sizeof(cmd), "%s makdir test_cli.dsk,EXISTING_FILE > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Delete nested directory with quiet mode */
	snprintf(cmd, sizeof(cmd), "%s deldir -q test_cli.dsk,DIR1/SUB1 > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Verify nested directory was removed */
	ec = _os9_open(&p, "test_cli.dsk,DIR1/SUB1", FAM_READ | FAM_DIR);
	ASSERT_EQUALS(EOS_PNNF, ec);
	ASSERT_TRUE(p == NULL);

	/* Delete parent directory */
	snprintf(cmd, sizeof(cmd), "%s deldir -q test_cli.dsk,DIR1 > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Delete nonexistent directory */
	snprintf(cmd, sizeof(cmd), "%s deldir -q test_cli.dsk,NONEXISTENT > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	remove("test_cli.dsk");
}

/* ---------- 9. Rename ---------- */

void test_os9_command_rename(void)
{
	char cmd[256];
	int status;
	os9_path_id p = NULL;
	error_code ec;
	FILE *fp;

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	fp = fopen("cli_a.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("rename test data", 1, 16, fp);
	fclose(fp);

	snprintf(cmd, sizeof(cmd), "%s copy cli_a.tmp test_cli.dsk,oldname.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Rename file */
	snprintf(cmd, sizeof(cmd), "%s rename test_cli.dsk,oldname.txt newname.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Verify old name is gone */
	ec = _os9_open(&p, "test_cli.dsk,oldname.txt", FAM_READ);
	ASSERT_EQUALS(EOS_PNNF, ec);
	ASSERT_TRUE(p == NULL);

	/* Verify new name exists */
	ec = _os9_open(&p, "test_cli.dsk,newname.txt", FAM_READ);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE(p != NULL);
	_os9_close(p);
	p = NULL;

	/* Rename nonexistent file must fail */
	snprintf(cmd, sizeof(cmd), "%s rename test_cli.dsk,nonexistent.txt dummy.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	remove("cli_a.tmp");
	remove("test_cli.dsk");
}

/* ---------- 10. Del ---------- */

void test_os9_command_del(void)
{
	char cmd[256];
	int status;
	os9_path_id p = NULL;
	error_code ec;
	FILE *fp;

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	fp = fopen("cli_a.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("del test", 1, 8, fp);
	fclose(fp);

	snprintf(cmd, sizeof(cmd), "%s copy cli_a.tmp test_cli.dsk,del1.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s copy cli_a.tmp test_cli.dsk,del2.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Delete single file */
	snprintf(cmd, sizeof(cmd), "%s del test_cli.dsk,del1.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	ec = _os9_open(&p, "test_cli.dsk,del1.bin", FAM_READ);
	ASSERT_EQUALS(EOS_PNNF, ec);
	ASSERT_TRUE(p == NULL);

	/* Delete remaining file */
	snprintf(cmd, sizeof(cmd), "%s del test_cli.dsk,del2.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	ec = _os9_open(&p, "test_cli.dsk,del2.bin", FAM_READ);
	ASSERT_EQUALS(EOS_PNNF, ec);
	ASSERT_TRUE(p == NULL);

	/* Delete nonexistent file must fail */
	snprintf(cmd, sizeof(cmd), "%s del test_cli.dsk,nonexistent.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	remove("cli_a.tmp");
	remove("test_cli.dsk");
}

/* ---------- 11. Attr ---------- */

void test_os9_command_attr(void)
{
	char cmd[256];
	int status;
	os9_path_id p = NULL;
	error_code ec;
	FILE *fp;
	int perms = 0;

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	fp = fopen("cli_a.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("attr test", 1, 9, fp);
	fclose(fp);

	snprintf(cmd, sizeof(cmd), "%s copy cli_a.tmp test_cli.dsk,attr.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Display attributes */
	snprintf(cmd, sizeof(cmd), "%s attr test_cli.dsk,attr.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Set execute and public write attributes */
	snprintf(cmd, sizeof(cmd), "%s attr -q -e -pw test_cli.dsk,attr.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	ec = _os9_open(&p, "test_cli.dsk,attr.bin", FAM_READ);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE(p != NULL);
	ec = _os9_gs_attr(p, &perms);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE((perms & FAP_EXEC) != 0);
	ASSERT_TRUE((perms & FAP_PWRITE) != 0);
	_os9_close(p);
	p = NULL;

	/* Clear execute attribute */
	snprintf(cmd, sizeof(cmd), "%s attr -q -ne test_cli.dsk,attr.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	ec = _os9_open(&p, "test_cli.dsk,attr.bin", FAM_READ);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE(p != NULL);
	perms = 0;
	ec = _os9_gs_attr(p, &perms);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE((perms & FAP_EXEC) == 0);
	_os9_close(p);
	p = NULL;

	/* attr on nonexistent file must fail with non-zero */
	snprintf(cmd, sizeof(cmd), "%s attr test_cli.dsk,nonexistent.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	remove("cli_a.tmp");
	remove("test_cli.dsk");
}

/* ---------- 12. Cmp ---------- */

void test_os9_command_cmp(void)
{
	char cmd[256];
	int status;
	FILE *fp;

	fp = fopen("cli_cmp1.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("IDENTICAL DATA", 1, 14, fp);
	fclose(fp);

	fp = fopen("cli_cmp2.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("IDENTICAL DATA", 1, 14, fp);
	fclose(fp);

	fp = fopen("cli_cmp3.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("DIFFERENT DATA", 1, 14, fp);
	fclose(fp);

	/* Compare identical files */
	snprintf(cmd, sizeof(cmd), "%s cmp cli_cmp1.tmp cli_cmp2.tmp > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Compare different files */
	snprintf(cmd, sizeof(cmd), "%s cmp cli_cmp1.tmp cli_cmp3.tmp > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Compare nonexistent files must fail with non-zero */
	snprintf(cmd, sizeof(cmd), "%s cmp nonexistent1.tmp nonexistent2.tmp > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	remove("cli_cmp1.tmp");
	remove("cli_cmp2.tmp");
	remove("cli_cmp3.tmp");
}

/* ---------- 13. Dcheck ---------- */

void test_os9_command_dcheck(void)
{
	char cmd[256];
	int status;
	FILE *fp;

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	fp = fopen("cli_a.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("check file", 1, 10, fp);
	fclose(fp);

	snprintf(cmd, sizeof(cmd), "%s copy cli_a.tmp test_cli.dsk,file.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* dcheck healthy disk */
	snprintf(cmd, sizeof(cmd), "%s dcheck test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* dcheck with options */
	snprintf(cmd, sizeof(cmd), "%s dcheck -s -b test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* dcheck on nonexistent disk */
	snprintf(cmd, sizeof(cmd), "%s dcheck nonexistent.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	remove("cli_a.tmp");
	remove("test_cli.dsk");
}

/* ---------- 14. Padrom ---------- */

void test_os9_command_padrom(void)
{
	char cmd[256];
	int status;
	FILE *fp;
	long sz;
	char padbuf[32];

	fp = fopen("cli_pad.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("1234", 1, 4, fp);
	fclose(fp);

	/* Pad to 16 bytes */
	snprintf(cmd, sizeof(cmd), "%s padrom 16 cli_pad.tmp > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	fp = fopen("cli_pad.tmp", "rb");
	ASSERT_TRUE(fp != NULL);
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	fclose(fp);
	ASSERT_EQUALS(16, sz);

	/* Pad at beginning with 'A' (ASCII 65) to 32 bytes */
	snprintf(cmd, sizeof(cmd), "%s padrom -b -c=65 32 cli_pad.tmp > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	fp = fopen("cli_pad.tmp", "rb");
	ASSERT_TRUE(fp != NULL);
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	memset(padbuf, 0, sizeof(padbuf));
	fread(padbuf, 1, sizeof(padbuf), fp);
	fclose(fp);

	ASSERT_EQUALS(32, sz);
	ASSERT_EQUALS('A', padbuf[0]);

	remove("cli_pad.tmp");
}

/* ---------- 15. Gen ---------- */

void test_os9_command_gen(void)
{
	char cmd[256];
	int status;
	FILE *fp;
	char buffer[100];

	remove("test_cli.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Create dummy bootfile */
	fp = fopen("cli_boot.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	memset(buffer, 0xAA, sizeof(buffer));
	fwrite(buffer, 1, sizeof(buffer), fp);
	fclose(fp);

	snprintf(cmd, sizeof(cmd), "%s gen -c test_cli.dsk -b=cli_boot.tmp > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Verify disk structure intact with dcheck */
	snprintf(cmd, sizeof(cmd), "%s dcheck test_cli.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	remove("cli_boot.tmp");
	remove("test_cli.dsk");
}

/*
 * Regression guard for the os9gen bitmap leak: running `os9 gen` repeatedly
 * overwrote the OS9Boot file and leaked allocation-bitmap bits, so free space
 * shrank on every run. Repeated runs must leave free space unchanged.
 */
void test_os9_gen_no_bitmap_leak(void)
{
	char cmd[256];
	int status;
	FILE *fp;
	char buffer[100];
	char dname[64];
	u_int month, day, year, bps, total_sectors, bytes_free;
	u_int free_1, free_2;
	u_int largest_free_block, sectors_per_cluster, largest_count, sector_count;
	error_code ec;

	remove("test_gen_leak.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_gen_leak.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Create a small boot file to link */
	fp = fopen("cli_boot.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	memset(buffer, 0xAA, sizeof(buffer));
	fwrite(buffer, 1, sizeof(buffer), fp);
	fclose(fp);

	/* First gen run */
	snprintf(cmd, sizeof(cmd), "%s gen -c test_gen_leak.dsk -b=cli_boot.tmp > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	ec = TSRBFFree("test_gen_leak.dsk", dname, &month, &day, &year, &bps,
	                &total_sectors, &bytes_free, &free_1,
	                &largest_free_block, &sectors_per_cluster,
	                &largest_count, &sector_count);
	ASSERT_EQUALS(0, ec);

	/* Second gen run over the same OS9Boot must not leak more bits */
	snprintf(cmd, sizeof(cmd), "%s gen -c test_gen_leak.dsk -b=cli_boot.tmp > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	ec = TSRBFFree("test_gen_leak.dsk", dname, &month, &day, &year, &bps,
	                &total_sectors, &bytes_free, &free_2,
	                &largest_free_block, &sectors_per_cluster,
	                &largest_count, &sector_count);
	ASSERT_EQUALS(0, ec);

	ASSERT_EQUALS(free_1, free_2);

	/* Disk must still be structurally intact */
	snprintf(cmd, sizeof(cmd), "%s dcheck test_gen_leak.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	remove("cli_boot.tmp");
	remove("test_gen_leak.dsk");
}

/*
 * Proactive integrity-churn harness. For both spc=1 and spc=2: fresh format,
 * then create small + large + fragmented (extended across close/reopen)
 * files, verify dcheck and free-space accounting at each phase, then
 * rename/delete churn, and finally confirm the exact baseline free sector
 * count is restored (no cluster or bitmap leak).
 */
void test_os9_integrity_stress(void)
{
	char dname[64];
	u_int month, day, year, bps, total_sectors, bytes_free;
	u_int free_baseline, free_now;
	u_int largest_free_block, sectors_per_cluster, largest_count, sector_count;
	unsigned int totalSectors, totalBytes;
	int clusterSize;
	int spc;
	os9_path_id p = NULL;
	error_code ec;
	char cmd[256];
	int status;
	char *data;
	u_int size;

	for (spc = 1; spc <= 2; spc++)
	{
		clusterSize = spc;
		remove("test_stress.dsk");

		ec = _os9_format("test_stress.dsk", 0, 80, 18, 18, 2, 256, &clusterSize,
		                    "Stress", 8, 8, 1, 1, 0, 0, 3, &totalSectors, &totalBytes);
		ASSERT_EQUALS(0, ec);

		ec = TSRBFFree("test_stress.dsk", dname, &month, &day, &year, &bps,
		                &total_sectors, &bytes_free, &free_baseline,
		                &largest_free_block, &sectors_per_cluster,
		                &largest_count, &sector_count);
		ASSERT_EQUALS(0, ec);
		ASSERT_EQUALS(spc, (int)sectors_per_cluster);

		/* small file (1 cluster) */
		data = malloc(100);
		ASSERT("small buffer", data != NULL);
		memset(data, 's', 100);
		ec = _os9_create(&p, "test_stress.dsk,small.txt",
		                 FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
		ASSERT_EQUALS(0, ec);
		size = 100;
		ec = _os9_write(p, data, &size);
		ASSERT_EQUALS(0, ec);
		ec = _os9_close(p);
		ASSERT_EQUALS(0, ec);
		p = NULL;
		free(data);

		/* large multi-cluster file */
		size = 20000;
		data = malloc(size);
		ASSERT("large buffer", data != NULL);
		memset(data, 'L', size);
		ec = _os9_create(&p, "test_stress.dsk,large.bin",
		                 FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
		ASSERT_EQUALS(0, ec);
		ec = _os9_write(p, data, &size);
		ASSERT_EQUALS(0, ec);
		ec = _os9_close(p);
		ASSERT_EQUALS(0, ec);
		p = NULL;

		/* fragmented file: extended across a close/reopen to exercise
		 * segment growth rather than a single contiguous allocation */
		ec = _os9_create(&p, "test_stress.dsk,frag.bin",
		                 FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
		ASSERT_EQUALS(0, ec);
		size = 2000;
		ec = _os9_write(p, data, &size);
		ASSERT_EQUALS(0, ec);
		ec = _os9_close(p);
		ASSERT_EQUALS(0, ec);
		p = NULL;

		ec = _os9_open(&p, "test_stress.dsk,frag.bin", FAM_READ | FAM_WRITE);
		ASSERT_EQUALS(0, ec);
		ec = _os9_seek(p, 0, SEEK_END);
		ASSERT_EQUALS(0, ec);
		size = 2000;
		ec = _os9_write(p, data, &size);
		ASSERT_EQUALS(0, ec);
		ec = _os9_close(p);
		ASSERT_EQUALS(0, ec);
		p = NULL;
		free(data);

		/* structure intact after writes */
		snprintf(cmd, sizeof(cmd), "%s dcheck test_stress.dsk > /dev/null 2>&1", get_os9_bin());
		status = system(cmd);
		ASSERT_EQUALS(0, status);

		/* rename churn */
		ec = _os9_rename("test_stress.dsk,small.txt", "small2.txt");
		ASSERT_EQUALS(0, ec);
		ec = _os9_rename("test_stress.dsk,small2.txt", "small.txt");
		ASSERT_EQUALS(0, ec);

		/* delete churn: remove everything we created */
		ec = _os9_delete("test_stress.dsk,small.txt");
		ASSERT_EQUALS(0, ec);
		ec = _os9_delete("test_stress.dsk,large.bin");
		ASSERT_EQUALS(0, ec);
		ec = _os9_delete("test_stress.dsk,frag.bin");
		ASSERT_EQUALS(0, ec);

		/* structure intact after deletes */
		snprintf(cmd, sizeof(cmd), "%s dcheck test_stress.dsk > /dev/null 2>&1", get_os9_bin());
		status = system(cmd);
		ASSERT_EQUALS(0, status);

		/* baseline free space fully restored (no leak) */
		ec = TSRBFFree("test_stress.dsk", dname, &month, &day, &year, &bps,
		                &total_sectors, &bytes_free, &free_now,
		                &largest_free_block, &sectors_per_cluster,
		                &largest_count, &sector_count);
		ASSERT_EQUALS(0, ec);
		ASSERT_EQUALS(free_baseline, free_now);

		remove("test_stress.dsk");
	}
}

/* ---------- 16. Ident ---------- */

void test_os9_command_ident(void)
{
	const char *disk = get_sample_disk();
	char cmd[512];
	int status;

	if (disk == NULL)
	{
		/* Skip if sample disk not present */
		return;
	}

	snprintf(cmd, sizeof(cmd), "%s ident -s %s,OS9Boot > /dev/null 2>&1", get_os9_bin(), disk);
	status = system(cmd);
	ASSERT_EQUALS(0, status);
}

/* ---------- 17. Multi-Cluster Allocation & Deletion ---------- */

void test_os9_cluster_allocation_and_deletion(void)
{
	char cmd[512];
	int status;
	FILE *fp;
	char buf[8192];

	memset(buf, 'A', sizeof(buf));

	/* Create 3 test files of various sizes:
	   - Small: 100 bytes (fits within the leftover sectors of the FD cluster)
	   - Medium: 1024 bytes (spans multiple sectors)
	   - Large: 4096 bytes (spans multiple clusters)
	*/
	fp = fopen("cli_cl_small.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite(buf, 1, 100, fp);
	fclose(fp);

	fp = fopen("cli_cl_med.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite(buf, 1, 1024, fp);
	fclose(fp);

	fp = fopen("cli_cl_large.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite(buf, 1, 4096, fp);
	fclose(fp);

	/* --- Part A: Disk with sectors_per_cluster = 2 (-c=2) --- */
	snprintf(cmd, sizeof(cmd), "%s format -q -c=2 test_clust2.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Copy small, medium, and large files to the disk */
	snprintf(cmd, sizeof(cmd), "%s copy cli_cl_small.tmp test_clust2.dsk,small > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s copy cli_cl_med.tmp test_clust2.dsk,med > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s copy cli_cl_large.tmp test_clust2.dsk,large > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Integrity check with dcheck */
	snprintf(cmd, sizeof(cmd), "%s dcheck test_clust2.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Delete medium file (regression test: multi-cluster file deletion on spc=2 disk) */
	snprintf(cmd, sizeof(cmd), "%s del test_clust2.dsk,med > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* dcheck must report 0 orphaned clusters */
	snprintf(cmd, sizeof(cmd), "%s dcheck test_clust2.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Delete small and large files */
	snprintf(cmd, sizeof(cmd), "%s del test_clust2.dsk,small > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s del test_clust2.dsk,large > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* dcheck must report completely clean disk */
	snprintf(cmd, sizeof(cmd), "%s dcheck test_clust2.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* --- Part B: Disk with sectors_per_cluster = 4 (-c=4) --- */
	snprintf(cmd, sizeof(cmd), "%s format -q -c=4 test_clust4.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s copy cli_cl_small.tmp test_clust4.dsk,small > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s copy cli_cl_large.tmp test_clust4.dsk,large > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s dcheck test_clust4.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Overwrite/truncate large file with small file (-r rewrite) to test _os9_truncate_seg_list */
	snprintf(cmd, sizeof(cmd), "%s copy -r cli_cl_small.tmp test_clust4.dsk,large > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s dcheck test_clust4.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Delete remaining files */
	snprintf(cmd, sizeof(cmd), "%s del test_clust4.dsk,small test_clust4.dsk,large > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s dcheck test_clust4.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	remove("cli_cl_small.tmp");
	remove("cli_cl_med.tmp");
	remove("cli_cl_large.tmp");
	remove("test_clust2.dsk");
	remove("test_clust4.dsk");
}

/* ---------- Command Line Parameter Error Handling ---------- */

void test_os9_command_line_parameters(void)
{
	char cmd[512];
	int status;
	FILE *fp;
	os9_path_id p = NULL;
	error_code ec;
	int perms = 0;

	/* 1. Unknown executive option */
	snprintf(cmd, sizeof(cmd), "%s -z > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* 2. Format parameter error handling */
	/* Missing target path */
	snprintf(cmd, sizeof(cmd), "%s format -e -q > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Unknown format option */
	snprintf(cmd, sizeof(cmd), "%s format -z test_badfmt.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Create a valid disk for subsequent tests */
	remove("test_param.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -t=35 test_param.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* 3. Copy parameter error handling */
	/* Missing arguments */
	snprintf(cmd, sizeof(cmd), "%s copy > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	snprintf(cmd, sizeof(cmd), "%s copy file1 > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Copying zero files to an existing directory is a valid no-op (Make variable expansion) */
	snprintf(cmd, sizeof(cmd), "%s makdir test_param.dsk,DIR > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s copy test_param.dsk,DIR > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	snprintf(cmd, sizeof(cmd), "%s copy -o=0 test_param.dsk,DIR > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Missing buffer argument to -b without crash */
	snprintf(cmd, sizeof(cmd), "%s copy -b file1 file2 > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Missing owner argument to -o without crash */
	snprintf(cmd, sizeof(cmd), "%s copy -o file1 file2 > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Unknown copy option */
	snprintf(cmd, sizeof(cmd), "%s copy -z file1 file2 > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Populate a file on the test disk */
	fp = fopen("cli_param.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("parameter test", 1, 14, fp);
	fclose(fp);

	snprintf(cmd, sizeof(cmd), "%s copy cli_param.tmp test_param.dsk,test.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* 4. Attr parameter error handling and multi-char options */
	/* Missing path */
	snprintf(cmd, sizeof(cmd), "%s attr > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	snprintf(cmd, sizeof(cmd), "%s attr -e > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Unknown attr option */
	snprintf(cmd, sizeof(cmd), "%s attr -z test_param.dsk,test.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Set public exec and public write */
	snprintf(cmd, sizeof(cmd), "%s attr -q -pe -pw test_param.dsk,test.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	ec = _os9_open(&p, "test_param.dsk,test.bin", FAM_READ);
	ASSERT_EQUALS(0, ec);
	ec = _os9_gs_attr(p, &perms);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE((perms & FAP_PEXEC) != 0);
	ASSERT_TRUE((perms & FAP_PWRITE) != 0);
	_os9_close(p);
	p = NULL;

	/* Reset public exec with -npe: must clear public exec without setting user or public exec! */
	snprintf(cmd, sizeof(cmd), "%s attr -q -npe test_param.dsk,test.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	ec = _os9_open(&p, "test_param.dsk,test.bin", FAM_READ);
	ASSERT_EQUALS(0, ec);
	perms = 0;
	ec = _os9_gs_attr(p, &perms);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE((perms & FAP_PEXEC) == 0);
	ASSERT_TRUE((perms & FAP_PWRITE) != 0);
	_os9_close(p);
	p = NULL;

	/* Reset public write with -npw */
	snprintf(cmd, sizeof(cmd), "%s attr -q -npw test_param.dsk,test.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	ec = _os9_open(&p, "test_param.dsk,test.bin", FAM_READ);
	ASSERT_EQUALS(0, ec);
	perms = 0;
	ec = _os9_gs_attr(p, &perms);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE((perms & FAP_PWRITE) == 0);
	_os9_close(p);
	p = NULL;

	/* 5. Dump parameter error handling */
	snprintf(cmd, sizeof(cmd), "%s dump > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	snprintf(cmd, sizeof(cmd), "%s dump -t > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	snprintf(cmd, sizeof(cmd), "%s dump -unknown test_param.dsk,test.bin > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* 6. Rdump parameter error handling */
	snprintf(cmd, sizeof(cmd), "%s rdump > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	snprintf(cmd, sizeof(cmd), "%s rdump -unknown cli_param.tmp > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* 7. Gen parameter error handling */
	snprintf(cmd, sizeof(cmd), "%s gen > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	snprintf(cmd, sizeof(cmd), "%s gen -b > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	snprintf(cmd, sizeof(cmd), "%s gen -z > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* 8. Ident parameter error handling */
	snprintf(cmd, sizeof(cmd), "%s ident > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	snprintf(cmd, sizeof(cmd), "%s ident -s > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	snprintf(cmd, sizeof(cmd), "%s ident -unknown cli_param.tmp > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* 9. Dsave parameter error handling */
	snprintf(cmd, sizeof(cmd), "%s dsave -b > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	remove("cli_param.tmp");
	remove("test_param.dsk");
}

/* ---------- Regression: rdump truncated file does not hang ---------- */

#ifndef ROFSYNC
#ifdef __APPLE__
#define ROFSYNC     0x62CD2387
#else
#define ROFSYNC     0x8723CD62
#endif
#endif

void test_os9_command_rdump_truncated_file_does_not_hang(void)
{
	char cmd[512];
	int status;
	FILE *fp;
	unsigned int sync = ROFSYNC;
	char zero[64];

	memset(zero, 0, sizeof(zero));

	fp = fopen("cli_trunc.r", "wb");
	ASSERT_TRUE(fp != NULL);
	/* Write ROFSYNC and zeroed header fields, truncated before module name null terminator */
	fwrite(&sync, sizeof(sync), 1, fp);
	fwrite(zero, 1, 32, fp);
	fclose(fp);

	/* Must finish promptly and not hang in an unbounded loop */
	snprintf(cmd, sizeof(cmd), "%s rdump cli_trunc.r > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	(void)status;

	/* Verify argv[0][0] fix: options like -a do not cause "can't open '-a'" */
	snprintf(cmd, sizeof(cmd), "%s rdump cli_trunc.r -a > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	(void)status;

	remove("cli_trunc.r");
}

/* ---------- Regression: copy rejects double-comma path ---------- */

void test_os9_command_copy_rejects_double_comma_path(void)
{
	char cmd[512];
	int status;
	FILE *fp;
	os9_path_id p = NULL;
	error_code ec;

	remove("test_dcomma.dsk");
	snprintf(cmd, sizeof(cmd), "%s format -e -q -ss test_dcomma.dsk > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Create directory SUB1 */
	snprintf(cmd, sizeof(cmd), "%s makdir test_dcomma.dsk,SUB1 > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	fp = fopen("cli_dcomma_src.tmp", "wb");
	ASSERT_TRUE(fp != NULL);
	fwrite("double comma test data\n", 1, 23, fp);
	fclose(fp);

	/* 1. Malformed destination with double comma must be rejected with non-zero exit status */
	snprintf(cmd, sizeof(cmd), "%s copy cli_dcomma_src.tmp test_dcomma.dsk,SUB1,dest.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_TRUE(status != 0);

	/* Verify no file was created under root or SUB1 */
	ec = _os9_open(&p, "test_dcomma.dsk,dest.txt", FAM_READ);
	ASSERT_TRUE(ec != 0);
	if (p != NULL) { _os9_close(p); p = NULL; }

	ec = _os9_open(&p, "test_dcomma.dsk,SUB1/dest.txt", FAM_READ);
	ASSERT_TRUE(ec != 0);
	if (p != NULL) { _os9_close(p); p = NULL; }

	/* 2. Valid slash path must succeed */
	snprintf(cmd, sizeof(cmd), "%s copy cli_dcomma_src.tmp test_dcomma.dsk,SUB1/dest.txt > /dev/null 2>&1", get_os9_bin());
	status = system(cmd);
	ASSERT_EQUALS(0, status);

	/* Verify file exists and was written correctly */
	ec = _os9_open(&p, "test_dcomma.dsk,SUB1/dest.txt", FAM_READ);
	ASSERT_EQUALS(0, ec);
	ASSERT_TRUE(p != NULL);
	_os9_close(p);
	p = NULL;

	remove("cli_dcomma_src.tmp");
	remove("test_dcomma.dsk");
}

void test_os9_command_copy_metadata()
{
	error_code ec;
	struct stat native_stat;
	os9_path_id os9path;
	fd_stats os9_fstat;
	int os9_fstat_size = sizeof(os9_fstat);
	fd_stats converted_native_fstat = {0};

	/* Detect reproducible build */
	char *sde_env = getenv("SOURCE_DATE_EPOCH");
	int use_sde = 0;
	time_t sde_val = 0;
	if (sde_env != NULL) {
		char *endptr;
		sde_val = strtoll(sde_env, &endptr, 10);
		if (*endptr == '\0' && sde_val >= 0) {
			use_sde = 1;
		}
	}
 
	/* Create a file on the disk image with known content */
	ec = _os9_create(&os9path, "test.dsk,meta.txt",
	                 FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
	ASSERT_EQUALS(0, ec);
	char *buf = "hello";
	u_int size = strlen(buf);
	ec = _os9_write(os9path, buf, &size);
	ASSERT_EQUALS(0, ec);
	ec = _os9_close(os9path);
	ASSERT_EQUALS(0, ec);
 
	/* Set the OS9 file's modification date to a fixed date in the 1980s:
	 * January 15, 1987 at 10:30 (OS9 fd_dat: year, month, day, hour, min).
	 * January is chosen to avoid DST ambiguity in the mktime round-trip. */
	ec = _os9_open(&os9path, "test.dsk,meta.txt", FAM_READ | FAM_WRITE);
	ASSERT_EQUALS(0, ec);
	ec = _os9_gs_fd(os9path, os9_fstat_size, &os9_fstat);
	ASSERT_EQUALS(0, ec);
	os9_fstat.fd_dat[0] = 87;  /* year: 1987 (years since 1900) */
	os9_fstat.fd_dat[1] = 1;   /* month: January */
	os9_fstat.fd_dat[2] = 15;  /* day */
	os9_fstat.fd_dat[3] = 10;  /* hour */
	os9_fstat.fd_dat[4] = 30;  /* minute */
	ec = _os9_ss_fd(os9path, os9_fstat_size, &os9_fstat);
	ASSERT_EQUALS(0, ec);
	ec = _os9_close(os9path);
	ASSERT_EQUALS(0, ec);
 
	/* Copy from OS9 image to native filesystem */
	ec = system("../os9/os9 copy test.dsk,meta.txt meta_out.txt > /dev/null 2>&1");
	ASSERT_EQUALS(0, ec);
 
	/* Read the OS9 file's modification time */
	ec = _os9_open(&os9path, "test.dsk,meta.txt", FAM_READ);
	ASSERT_EQUALS(0, ec);
	ec = _os9_gs_fd(os9path, os9_fstat_size, &os9_fstat);
	ASSERT_EQUALS(0, ec);
	ec = _os9_close(os9path);
	ASSERT_EQUALS(0, ec);
 
	/* Check that the native file's mtime matches the OS9 last_modified_time */
	ec = stat("meta_out.txt", &native_stat);
	ASSERT_EQUALS(0, ec);
	UnixToOS9Time(native_stat.st_mtime, (char *) converted_native_fstat.fd_dat);
	
// 	fprintf(stderr, "os9_fstat.fd_dat: %d %d %d %d %d\n", os9_fstat.fd_dat[0], os9_fstat.fd_dat[1],os9_fstat.fd_dat[2], os9_fstat.fd_dat[3], os9_fstat.fd_dat[4]);
// 	fprintf(stderr, "converted_native_fstat.fd_dat: %d %d %d %d %d\n", converted_native_fstat.fd_dat[0], converted_native_fstat.fd_dat[1],converted_native_fstat.fd_dat[2], converted_native_fstat.fd_dat[3], converted_native_fstat.fd_dat[4]);
	
	if (use_sde) {
		/* Reproducible mode: Destination is pinned to SOURCE_DATE_EPOCH */
		ASSERT_EQUALS(sde_val, native_stat.st_mtime);

		struct tm *expected_tm = gmtime(&sde_val);
		fd_stats expected_from_sde = {0};
		expected_from_sde.fd_dat[0] = expected_tm->tm_year;
		expected_from_sde.fd_dat[1] = expected_tm->tm_mon + 1;
		expected_from_sde.fd_dat[2] = expected_tm->tm_mday;
		expected_from_sde.fd_dat[3] = expected_tm->tm_hour;
		expected_from_sde.fd_dat[4] = expected_tm->tm_min;

		ASSERT_MEM_EQUALS(expected_from_sde.fd_dat, converted_native_fstat.fd_dat, 5);
	} else {
		ASSERT_MEM_EQUALS(os9_fstat.fd_dat, converted_native_fstat.fd_dat, 5);
	}

	/* Change the native file's mtime to a different fixed date:
     * March 3, 1984 at 14:15 */
    struct utimbuf native_utime;
    struct tm native_tm;
    memset(&native_tm, 0, sizeof(native_tm));
    native_tm.tm_year = 84;  /* 1984 */
    native_tm.tm_mon  = 2;   /* March */
    native_tm.tm_mday = 3;
    native_tm.tm_hour = 14;
    native_tm.tm_min  = 15;
    native_tm.tm_isdst = -1;
    native_utime.modtime = mktime(&native_tm);
    native_utime.actime  = native_utime.modtime;
    ec = utime("meta_out.txt", &native_utime);
    ASSERT_EQUALS(0, ec);

    /* Copy native file back into the disk image */
    ec = system("../os9/os9 copy meta_out.txt test.dsk,meta_back.txt > /dev/null 2>&1");
    ASSERT_EQUALS(0, ec);

    /* Read the OS9 file's modification time */
    fd_stats os9_back_fstat;
    ec = _os9_open(&os9path, "test.dsk,meta_back.txt", FAM_READ);
    ASSERT_EQUALS(0, ec);
    ec = _os9_gs_fd(os9path, os9_fstat_size, &os9_back_fstat);
    ASSERT_EQUALS(0, ec);
    ec = _os9_close(os9path);
    ASSERT_EQUALS(0, ec);

    /* Check that the OS9 file's mtime matches the native file's mtime */
    fd_stats expected_back_fstat = {0};
	if (use_sde) {
		UnixToOS9Time(sde_val, (char *) expected_back_fstat.fd_dat);
	} else {
		UnixToOS9Time(native_utime.modtime, (char *) expected_back_fstat.fd_dat);
	}

// 	fprintf(stderr, "os9_back_fstat.fd_dat: %d %d %d %d %d\n", os9_back_fstat.fd_dat[0], os9_back_fstat.fd_dat[1],os9_back_fstat.fd_dat[2], os9_back_fstat.fd_dat[3], os9_back_fstat.fd_dat[4]);
// 	fprintf(stderr, "expected_back_fstat.fd_dat: %d %d %d %d %d\n", expected_back_fstat.fd_dat[0], expected_back_fstat.fd_dat[1],expected_back_fstat.fd_dat[2], expected_back_fstat.fd_dat[3], expected_back_fstat.fd_dat[4]);

    ASSERT_MEM_EQUALS(expected_back_fstat.fd_dat, os9_back_fstat.fd_dat, 5);

	remove("meta_out.txt");
}

void test_os9_command_copy_exit_status()
{
	error_code ec;
	FILE *file;
	coco_path_id path;
	const char contents[] = "copy status regression\n";
	char result[sizeof(contents)];
	size_t written;
	u_int size;
	int i;
	const char *commands[] = {
		"../os9/os9 copy copy_status_source copy_status.dsk,ONE.TXT > /dev/null 2>&1",
		"../os9/os9 copy copy_status_missing copy_status.dsk,MISSING.TXT > /dev/null 2>&1",
		"../os9/os9 copy copy_status_source copy_status.dsk,ONE.TXT > /dev/null 2>&1",
		"../os9/os9 copy copy_status_missing copy_status_source copy_status.dsk, > /dev/null 2>&1",
		"../os9/os9 copy -r copy_status_source copy_status_missing copy_status.dsk, > /dev/null 2>&1"
	};
	const int expected[] = {0, EOS_PNNF, EOS_FAE, EOS_PNNF, EOS_PNNF};

	ec = system("../os9/os9 format -e copy_status.dsk -l630 > /dev/null 2>&1");
	ASSERT_EQUALS(0, ec);
	remove("copy_status_missing");
	file = fopen("copy_status_source", "wb");
	ASSERT("create copy source", file != NULL);
	written = fwrite(contents, 1, sizeof(contents) - 1, file);
	ec = fclose(file);
	ASSERT_EQUALS(sizeof(contents) - 1, written);
	ASSERT_EQUALS(0, ec);

	for (i = 0; i < 5; i++)
	{
		ec = system(commands[i]);
		ASSERT_EQUALS(1, WIFEXITED(ec));
		ec = WEXITSTATUS(ec);
		ASSERT_EQUALS(expected[i], ec);

		/* A failed source must not prevent a later valid source from copying. */
		if (i == 3)
		{
			ec = _coco_open(&path, "copy_status.dsk,copy_status_source", FAM_READ);
			ASSERT_EQUALS(0, ec);
			size = sizeof(result);
			ec = _coco_read(path, result, &size);
			_coco_close(path);
			ASSERT_EQUALS(0, ec);
			ASSERT_EQUALS(sizeof(contents) - 1, size);
			ASSERT_MEM_EQUALS(contents, result, size);
		}
	}

	remove("copy_status_source");
	remove("copy_status.dsk");
}

/* A format that fails must report it, and leave an existing image alone. */
void test_os9_command_format_rejects_bad_t0s()
{
	FILE *file;
	int ec;

	file = fopen("bad_t0.dsk", "wb");
	ASSERT("create existing image", file != NULL);
	fputs("keep", file);
	fclose(file);

	ec = system("../os9/os9 format -q -st18 -sz19 bad_t0.dsk > /dev/null 2>&1");
	ASSERT_EQUALS(1, WIFEXITED(ec));
	ASSERT_NEQUALS(0, WEXITSTATUS(ec));

	file = fopen("bad_t0.dsk", "rb");
	ASSERT("reopen existing image", file != NULL);
	ASSERT_EQUALS(0, fseek(file, 0, SEEK_END));
	ASSERT_EQUALS(4, ftell(file));
	fclose(file);
	ec = remove("bad_t0.dsk");
	ASSERT_EQUALS(0, ec);

}

/* A root directory past T0S must still be identified as an OS-9 image, or the
 * tools treat the disk as Disk BASIC. Copy a file in and back out to prove it.
 */
void test_os9_command_root_past_short_t0()
{
	const char *formats[] = {
		"../os9/os9 format -q -t4 -ds -st18 -sz1 min_t0.dsk > /dev/null 2>&1",
		"../os9/os9 format -q -t4 -ds -st18 -sz2 min_t0.dsk > /dev/null 2>&1",
		"../os9/os9 format -q -e -k -t4 -ds -st18 -sz2 min_t0.dsk > /dev/null 2>&1",
		"../os9/os9 format -q -dr -t8 -ds -st18 -sz14 min_t0.dsk > /dev/null 2>&1"
	};
	const char contents[] = "root past track 0\n";
	char result[sizeof(contents)];
	FILE *file;
	size_t count;
	int ec, i;

	file = fopen("min_t0_source", "wb");
	ASSERT("create copy source", file != NULL);
	count = fwrite(contents, 1, sizeof(contents) - 1, file);
	fclose(file);
	ASSERT_EQUALS(sizeof(contents) - 1, count);

	for (i = 0; i < 4; i++)
	{
		remove("min_t0.dsk");
		remove("min_t0_result");
		ec = system(formats[i]);
		ASSERT_EQUALS(0, ec);
		ec = system("../os9/os9 dir min_t0.dsk, > /dev/null 2>&1");
		ASSERT_EQUALS(0, ec);
		ec = system("../os9/os9 copy min_t0_source min_t0.dsk,FILE > /dev/null 2>&1");
		ASSERT_EQUALS(0, ec);
		ec = system("../os9/os9 copy min_t0.dsk,FILE min_t0_result > /dev/null 2>&1");
		ASSERT_EQUALS(0, ec);
		file = fopen("min_t0_result", "rb");
		ASSERT("open copy result", file != NULL);
		count = fread(result, 1, sizeof(result), file);
		fclose(file);
		ASSERT_EQUALS(sizeof(contents) - 1, count);
		ASSERT_MEM_EQUALS(contents, result, count);
	}

	remove("min_t0.dsk");
	remove("min_t0_result");
	remove("min_t0_source");
}

void test_os9_command_no_comma_spec()
{
	error_code ec;
	os9_path_id path;
	char *filename = NULL;
	int i;
	const char *const commands[] = {
		"../os9/os9 del no_comma.dsk > /dev/null 2>&1",
		"../os9/os9 attr no_comma.dsk > /dev/null 2>&1",
		"../os9/os9 attr -e no_comma.dsk > /dev/null 2>&1",
		"../os9/os9 attr no_comma.dsk no_comma.dsk,KEEP > /dev/null 2>&1",
		"../os9/os9 attr no_comma.dsk,KEEP no_comma.dsk > /dev/null 2>&1",
		"../os9/os9 attr no_comma.dsk,NOPE no_comma.dsk > /dev/null 2>&1",
		"../os9/os9 attr no_comma.dsk,KEEP > /dev/null 2>&1",
	};
	const int expected[] = {EOS_BPNAM, EOS_BPNAM, EOS_BPNAM, EOS_BPNAM,
		EOS_BPNAM, EOS_PNNF, 0};

	remove("no_comma.dsk");
	remove("no_comma.before");

	ec = system("../os9/os9 format -q -t35 no_comma.dsk > /dev/null 2>&1");
	ASSERT_EQUALS(0, ec);
	ec = _os9_create(&path, "no_comma.dsk,KEEP", FAM_WRITE, FAP_READ | FAP_WRITE);
	ASSERT_EQUALS(0, ec);
	ec = _os9_close(path);
	ASSERT_EQUALS(0, ec);
	ec = _os9_create(&path, "no_comma.dsk,GONE", FAM_WRITE, FAP_READ | FAP_WRITE);
	ASSERT_EQUALS(0, ec);
	ec = _os9_close(path);
	ASSERT_EQUALS(0, ec);
	ec = system("cp no_comma.dsk no_comma.before");
	ASSERT_EQUALS(0, ec);

	/* A spec without a comma names no file in an image, so it must fail
	 * without crashing and without changing the image. */
	for (i = 0; i < 7; i++)
	{
		ec = system(commands[i]);
		ASSERT_EQUALS(1, WIFEXITED(ec));
		ec = WEXITSTATUS(ec);
		ASSERT_EQUALS(expected[i], ec);
		ec = system("cmp -s no_comma.dsk no_comma.before");
		ASSERT_EQUALS(0, ec);
	}

	ec = _os9_open_parent_directory(&path, "no_comma.dsk", FAM_DIR | FAM_READ,
					&filename);
	ASSERT_EQUALS(EOS_BPNAM, ec);
	ASSERT("no filename returned", filename == NULL);

	/* Specs before the bad one are still processed, and del stops at it. */
	ec = system("../os9/os9 del no_comma.dsk,GONE no_comma.dsk no_comma.dsk,KEEP > /dev/null 2>&1");
	ASSERT_EQUALS(1, WIFEXITED(ec));
	ec = WEXITSTATUS(ec);
	ASSERT_EQUALS(EOS_BPNAM, ec);
	ec = _os9_open(&path, "no_comma.dsk,GONE", FAM_READ);
	ASSERT_EQUALS(EOS_PNNF, ec);
	ec = _os9_open(&path, "no_comma.dsk,KEEP", FAM_READ);
	ASSERT_EQUALS(0, ec);
	ec = _os9_close(path);
	ASSERT_EQUALS(0, ec);

	remove("no_comma.before");
	remove("no_comma.dsk");
}

static long file_contents(const char *pathname, u_char *buffer, long size)
{
	FILE *fp = fopen(pathname, "rb");
	long got;

	if (fp == NULL)
	{
		return -1;
	}
	got = (long) fread(buffer, 1, size, fp);
	fclose(fp);

	return got;
}

/* A raw path is the whole disk, and has no file descriptor to take details from. */
void test_os9_command_copy_raw_path()
{
	static u_char image[161280], copy[161280];
	struct stat st;
	time_t started = time(NULL);
	error_code ec;
	long size;

	remove("test_raw.dsk");
	remove("test_raw.bin");

	ec = system("../os9/os9 format -q -e -t35 -st18 test_raw.dsk > /dev/null 2>&1");
	ASSERT_EQUALS(0, ec);

	ec = system("../os9/os9 copy test_raw.dsk,@ test_raw.bin > /dev/null 2>&1");
	ASSERT_EQUALS(0, ec);

	ASSERT_EQUALS(0, stat("test_raw.bin", &st));
	ASSERT("the owner can read the copy", (st.st_mode & S_IRUSR) != 0);
	ASSERT("the copy has no date from the disk", st.st_mtime >= started - 2);

	size = file_contents("test_raw.dsk", image, sizeof(image));
	ASSERT_EQUALS(161280, size);
	size = file_contents("test_raw.bin", copy, sizeof(copy));
	ASSERT_EQUALS(161280, size);
	ASSERT_EQUALS(0, memcmp(image, copy, sizeof(image)));

	/* The disk says it is longer than the file: a sector it needs is missing. */
	ASSERT_EQUALS(0, truncate("test_raw.dsk", 100 * 256));
	remove("test_raw.bin");
	ec = system("../os9/os9 copy test_raw.dsk,@ test_raw.bin > /dev/null 2>&1");
	ASSERT_NEQUALS(0, ec);

	remove("test_raw.dsk");
	remove("test_raw.bin");
}

/* -l only changes line endings between native files and disk images. */
void test_os9_command_copy_eol_same_type()
{
	static const u_char text[] = "one\rtwo\r";
	u_char back[sizeof(text)];
	os9_path_id p;
	error_code ec;
	u_int size = sizeof(text) - 1;
	long got;

	remove("test_eol.dsk");
	remove("test_eol.txt");
	remove("test_eol2.txt");

	ec = system("../os9/os9 format -q -e -t35 -st18 test_eol.dsk > /dev/null 2>&1");
	ASSERT_EQUALS(0, ec);
	ec = _os9_create(&p, "test_eol.dsk,A", FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
	ASSERT_EQUALS(0, ec);
	ec = _os9_write(p, (void *) text, &size);
	ASSERT_EQUALS(0, ec);
	ec = _os9_close(p);
	ASSERT_EQUALS(0, ec);

	/* Disk image to disk image */
	ec = system("../os9/os9 copy -l test_eol.dsk,A test_eol.dsk,B > /dev/null 2>&1");
	ASSERT_EQUALS(0, ec);
	ec = _os9_open(&p, "test_eol.dsk,B", FAM_READ);
	ASSERT_EQUALS(0, ec);
	size = sizeof(back);
	ec = _os9_read(p, back, &size);
	ASSERT_EQUALS(0, ec);
	ASSERT_EQUALS(sizeof(text) - 1, size);
	ASSERT_EQUALS(0, memcmp(text, back, size));
	ec = _os9_close(p);
	ASSERT_EQUALS(0, ec);

	/* Native file to native file */
	ec = system("printf 'one\\ntwo\\n' > test_eol.txt");
	ASSERT_EQUALS(0, ec);
	ec = system("../os9/os9 copy -l test_eol.txt test_eol2.txt > /dev/null 2>&1");
	ASSERT_EQUALS(0, ec);
	got = file_contents("test_eol2.txt", back, sizeof(back));
	ASSERT_EQUALS(8, got);
	ASSERT_EQUALS(0, memcmp("one\ntwo\n", back, got));

	remove("test_eol.dsk");
	remove("test_eol.txt");
	remove("test_eol2.txt");
}

/* A copy never replaces a host file that looks like a disk image, even with -r. */
void test_os9_command_copy_refuses_disk_images()
{
	static const char *const commands[] = {
		"../os9/os9 copy test_guard_src.txt %s > /dev/null 2> test_guard_err.txt",
		"../os9/os9 copy -r test_guard_src.txt %s > /dev/null 2> test_guard_err.txt",
	};
	static const char *const targets[] = { "test_guard.dsk", "test_guard.bin" };
	static char before[4608], after[4608], message[512];
	FILE *fp;
	size_t i, got;
	int t, status;

	fp = fopen("test_guard_src.txt", "wb");
	fputs("replacement\n", fp);
	fclose(fp);

	for (t = 0; t < 2; t++)
	{
		char command[256];

		/* One looks like an image by name, the other by its size. */
		memset(before, 0x5A, sizeof(before));
		fp = fopen(targets[t], "wb");
		fwrite(before, 1, sizeof(before), fp);
		fclose(fp);

		for (i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
		{
			snprintf(command, sizeof(command), commands[i], targets[t]);
			status = system(command);
			ASSERT_NEQUALS(0, status);

			fp = fopen("test_guard_err.txt", "rb");
			got = fread(message, 1, sizeof(message) - 1, fp);
			fclose(fp);
			message[got] = 0;
			ASSERT("it says why", strstr(message, "looks like a disk image") != NULL);

			fp = fopen(targets[t], "rb");
			got = fread(after, 1, sizeof(after), fp);
			fclose(fp);
			ASSERT_EQUALS(sizeof(before), got);
			ASSERT_EQUALS(0, memcmp(before, after, sizeof(before)));
		}
		remove(targets[t]);
	}

	/* Other files are still written. */
	fp = fopen("test_guard.txt", "wb");
	fputs("old\n", fp);
	fclose(fp);
	status = system("../os9/os9 copy -r test_guard_src.txt test_guard.txt > /dev/null 2>&1");
	ASSERT_EQUALS(0, status);
	fp = fopen("test_guard.txt", "rb");
	got = fread(after, 1, sizeof(after), fp);
	fclose(fp);
	ASSERT_EQUALS(12, got);

	remove("test_guard.txt");
	remove("test_guard_src.txt");
	remove("test_guard_err.txt");
}

/* dsave runs copy, so it refuses too, and its own exit status says so. */
void test_os9_command_dsave_refuses_disk_images()
{
	static char before[4608], after[4608];
	FILE *fp;
	size_t got;
	int status;

	status = system("rm -rf test_dsave_src test_dsave_dst && mkdir test_dsave_src test_dsave_dst"
			" && echo new > test_dsave_src/x.dsk");
	ASSERT_EQUALS(0, status);
	memset(before, 0x5A, sizeof(before));
	fp = fopen("test_dsave_dst/x.dsk", "wb");
	fwrite(before, 1, sizeof(before), fp);
	fclose(fp);

	status = system("PATH=../os9:$PATH ../os9/os9 dsave -e -r test_dsave_src test_dsave_dst > /dev/null 2>&1");
	ASSERT_NEQUALS(0, status);
	ASSERT_EQUALS(EOS_FAE, WEXITSTATUS(status));

	fp = fopen("test_dsave_dst/x.dsk", "rb");
	got = fread(after, 1, sizeof(after), fp);
	fclose(fp);
	ASSERT_EQUALS(sizeof(before), got);
	ASSERT_EQUALS(0, memcmp(before, after, sizeof(before)));

	status = system("rm -rf test_dsave_src test_dsave_dst");
	ASSERT_EQUALS(0, status);
}

/* -x names the executable, in the commands shown and those run. */
void test_os9_command_dsave_executable()
{
	static char output[4096];
	FILE *fp;
	size_t got;
	int status;

	status = system("rm -rf test_x_src test_x_dst && mkdir test_x_src test_x_dst"
			" && echo one > test_x_src/one.txt");
	ASSERT_EQUALS(0, status);

	/* Shown only, under its usual name by default */
	status = system("../os9/os9 dsave test_x_src test_x_dst > test_x_out.txt 2>&1");
	ASSERT_EQUALS(0, status);
	fp = fopen("test_x_out.txt", "rb");
	got = fread(output, 1, sizeof(output) - 1, fp);
	fclose(fp);
	output[got] = 0;
	ASSERT("it shows the usual name", strncmp(output, "os9 copy 'test_x_src/one.txt' 'test_x_dst/one.txt'", 44) == 0);
	fp = fopen("test_x_dst/one.txt", "rb");
	ASSERT("nothing was copied", fp == NULL);

	/* Shown only, under the given name */
	status = system("../os9/os9 dsave -x=no/such/os9 test_x_src test_x_dst > test_x_out.txt 2>&1");
	ASSERT_EQUALS(0, status);
	fp = fopen("test_x_out.txt", "rb");
	got = fread(output, 1, sizeof(output) - 1, fp);
	fclose(fp);
	output[got] = 0;
	ASSERT("it shows the given name", strncmp(output, "'no/such/os9' copy ", 18) == 0);

	/* An empty search path shows it is not the shell finding it. */
	status = system("PATH=/nonexistent:/bin:/usr/bin ../os9/os9 dsave -e -x=../os9/os9 test_x_src test_x_dst > /dev/null 2>&1");
	ASSERT_EQUALS(0, status);
	fp = fopen("test_x_dst/one.txt", "rb");
	ASSERT("the file was copied", fp != NULL);
	got = fread(output, 1, sizeof(output) - 1, fp);
	fclose(fp);
	ASSERT_EQUALS(4, got);

	/* One that isn't there fails, and says so in the exit status */
	status = system("../os9/os9 dsave -e -r -x=no/such/os9 test_x_src test_x_dst > /dev/null 2>&1");
	ASSERT_NEQUALS(0, status);
	ASSERT_NEQUALS(0, WEXITSTATUS(status));

	/* It needs a path */
	status = system("../os9/os9 dsave -x test_x_src test_x_dst > /dev/null 2>&1");
	ASSERT_NEQUALS(0, status);

	status = system("rm -rf test_x_src test_x_dst test_x_out.txt");
	ASSERT_EQUALS(0, status);
}

/* Check a format makes the format asked for, and a disk that works. */
static void check_format_format(const char *arguments, const char *pathname, diskimage_format expected)
{
	static u_char text[3000], back[3000];
	char command[256];
	char pathlist[128];
	diskimage_format format = DISKIMAGE_UNKNOWN;
	os9_path_id p;
	u_int size = sizeof(text);
	u_int filesystems;
	error_code ec;
	int status, i;

	for (i = 0; i < (int) sizeof(text); i++)
	{
		text[i] = (u_char) (i * 7);
	}

	remove(pathname);
	snprintf(command, sizeof(command), "../os9/os9 format -q %s %s > /dev/null 2>&1", arguments, pathname);
	status = system(command);
	ASSERT_EQUALS(0, status);

	ec = _diskimage_identify(pathname, &format);
	ASSERT_EQUALS(0, ec);
	ASSERT_EQUALS(expected, format);

	ec = _coco_detect_filesystems(pathname, &filesystems);
	ASSERT_EQUALS(0, ec);
	ASSERT_EQUALS(COCO_FS_OS9, filesystems);

	snprintf(pathlist, sizeof(pathlist), "%s,FILE", pathname);
	ec = _os9_create(&p, pathlist, FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
	ASSERT_EQUALS(0, ec);
	ec = _os9_write(p, text, &size);
	ASSERT_EQUALS(0, ec);
	ec = _os9_close(p);
	ASSERT_EQUALS(0, ec);

	ec = _os9_open(&p, pathlist, FAM_READ);
	ASSERT_EQUALS(0, ec);
	size = sizeof(back);
	ec = _os9_read(p, back, &size);
	ASSERT_EQUALS(0, ec);
	ASSERT_EQUALS(sizeof(text), size);
	ASSERT_EQUALS(0, memcmp(text, back, size));
	ec = _os9_close(p);
	ASSERT_EQUALS(0, ec);

	snprintf(command, sizeof(command), "../os9/os9 dcheck %s > /dev/null 2>&1", pathname);
	status = system(command);
	ASSERT_EQUALS(0, status);
	remove(pathname);
}

void test_os9_command_format_formats()
{
	int status;

	/* By extension */
	check_format_format("", "test_fmt.dmk", DISKIMAGE_DMK);
	check_format_format("", "test_fmt.SDF", DISKIMAGE_SDF);
	check_format_format("", "test_fmt.vdk", DISKIMAGE_VDK);
	check_format_format("-e", "test_fmt.dsk", DISKIMAGE_RAW);
	/* A JVC image of this layout needs no header, and so is raw. */
	check_format_format("", "test_fmt.jvc", DISKIMAGE_RAW);

	/* The option wins */
	check_format_format("-fdmk", "test_fmt.dsk", DISKIMAGE_DMK);
	check_format_format("-e -fraw", "test_fmt.dmk", DISKIMAGE_RAW);

	/* Other layouts */
	check_format_format("-ds", "test_fmt.dmk", DISKIMAGE_DMK);
	check_format_format("-t40 -ds", "test_fmt.vdk", DISKIMAGE_VDK);
	check_format_format("-sz10", "test_fmt.dmk", DISKIMAGE_DMK);
	check_format_format("-ds -sz10", "test_fmt.dmk", DISKIMAGE_DMK);
	check_format_format("-bs512 -st9", "test_fmt.dmk", DISKIMAGE_DMK);
	check_format_format("-st10 -t80 -ds", "test_fmt.jvc", DISKIMAGE_JVC);
	check_format_format("-dr", "test_fmt.dmk", DISKIMAGE_DMK);

	/* An existing file is not replaced by these formats */
	remove("test_fmt.dmk");
	status = system("../os9/os9 format -q test_fmt.dmk > /dev/null 2>&1");
	ASSERT_EQUALS(0, status);
	status = system("../os9/os9 format -q test_fmt.dmk > /dev/null 2>&1");
	ASSERT_NEQUALS(0, status);
	remove("test_fmt.dmk");

	/* A layout the format can't hold, and an unknown format, are refused */
	status = system("../os9/os9 format -q -sz10 test_fmt.vdk > /dev/null 2>&1");
	ASSERT_NEQUALS(0, status);

	/* 18 sectors of 512 bytes don't fit on a track. */
	status = system("../os9/os9 format -q -bs512 test_fmt.dmk > /dev/null 2>&1");
	ASSERT_NEQUALS(0, status);
	status = system("../os9/os9 format -q -sd test_fmt.dmk > /dev/null 2>&1");
	ASSERT_NEQUALS(0, status);
	status = system("../os9/os9 format -q -fxyz test_fmt.dmk > /dev/null 2>&1");
	ASSERT_NEQUALS(0, status);
}

/* The commands, not only the library, on an image with tracks. */
static void check_os9_commands(const char *image)
{
	static const char *const steps[] = {
		"format -q -e %s",
		"copy test_cmd_src.txt %s,FILE",
		"copy -l test_cmd_src.txt %s,TEXT",
		"makdir %s,DIR",
		"copy test_cmd_src.txt %s,DIR/INNER",
		"dir -e -r %s,",
		"dump %s,FILE",
		"list %s,TEXT",
		"free %s",
		"attr %s,FILE",
		"attr -w %s,FILE",
		"attr %s,DIR/INNER",
		"rename %s,FILE RENAMED",
		"reveal %s 2",
		"dcheck %s",
		"copy %s,RENAMED test_cmd_out.bin",
		"del %s,TEXT",
		"deldir -q %s,DIR",
	};
	char command[256];
	size_t i;
	int status;

	remove(image);
	remove("test_cmd_out.bin");
	for (i = 0; i < sizeof(steps) / sizeof(steps[0]); i++)
	{
		char step[128];

		snprintf(step, sizeof(step), steps[i], image);
		snprintf(command, sizeof(command), "../os9/os9 %s > test_cmd_log.txt 2>&1", step);
		status = system(command);
		if (status != 0)
		{
			printf("failed: %s\n", command);
		}
		ASSERT_EQUALS(0, status);
	}
	status = system("cmp test_cmd_src.txt test_cmd_out.bin");
	ASSERT_EQUALS(0, status);
	remove(image);
	remove("test_cmd_out.bin");
	remove("test_cmd_log.txt");
}

void test_os9_commands_on_images_with_tracks()
{
	FILE *fp = fopen("test_cmd_src.txt", "wb");
	int i;

	for (i = 0; i < 200; i++)
	{
		fputs("a line of text\n", fp);
	}
	fclose(fp);

	check_os9_commands("test_cmd.dmk");
	check_os9_commands("test_cmd.sdf");
	check_os9_commands("test_cmd.vdk");
	remove("test_cmd_src.txt");
}

/* gen links a boot file and writes the kernel track through a raw path. */
static void check_os9_gen(const char *image)
{
	static char boot[2048], track[4608];
	os9_path_id p;
	char command[256];
	u_char sector[256];
	error_code ec;
	FILE *fp;
	int status;

	memset(boot, 0x87, sizeof(boot));
	memset(track, 0x4F, sizeof(track));
	fp = fopen("test_gen_boot.bin", "wb");
	fwrite(boot, 1, sizeof(boot), fp);
	fclose(fp);
	fp = fopen("test_gen_track.bin", "wb");
	fwrite(track, 1, sizeof(track), fp);
	fclose(fp);

	remove(image);
	snprintf(command, sizeof(command), "../os9/os9 format -q -t40 %s > /dev/null 2>&1", image);
	status = system(command);
	ASSERT_EQUALS(0, status);
	snprintf(command, sizeof(command),
		 "../os9/os9 gen -b=test_gen_boot.bin -t=test_gen_track.bin %s > /dev/null 2>&1", image);
	status = system(command);
	ASSERT_EQUALS(0, status);

	snprintf(command, sizeof(command), "%s,@", image);
	ec = _os9_open(&p, command, FAM_READ);
	ASSERT_EQUALS(0, ec);
	ASSERT_NEQUALS(0, int3(p->lsn0->dd_bt));
	ASSERT_EQUALS(sizeof(boot), int2(p->lsn0->dd_bsz));

	/* The kernel track is track 34, whatever the image. */
	ec = _os9_lsn_get(p, 34 * 18, 0, sector, sizeof(sector));
	ASSERT_EQUALS(0, ec);
	ASSERT_EQUALS(0x4F, sector[0]);

	ec = _os9_lsn_get(p, int3(p->lsn0->dd_bt), 0, sector, sizeof(sector));
	ASSERT_EQUALS(0, ec);
	ASSERT_EQUALS(0x87, sector[0]);
	ec = _os9_close(p);
	ASSERT_EQUALS(0, ec);

	remove(image);
	remove("test_gen_boot.bin");
	remove("test_gen_track.bin");
}

void test_os9_gen_on_images_with_tracks()
{
	check_os9_gen("test_gen.dsk");
	check_os9_gen("test_gen.dmk");
	check_os9_gen("test_gen.sdf");
}

static long read_file(const char *pathname, u_char *buffer, size_t size)
{
	FILE *fp = fopen(pathname, "rb");
	long got;

	if (fp == NULL)
	{
		return -1;
	}
	got = (long) fread(buffer, 1, size, fp);
	fclose(fp);

	return got;
}

/* How many bytes in a range are not the value. */
static long count_other(const u_char *buffer, long start, long end, u_char value)
{
	long count = 0;
	long i;

	for (i = start; i < end; i++)
	{
		count += buffer[i] != value;
	}

	return count;
}

static void format_gap_image(const char *arguments, const char *image, u_char *before, long *before_size)
{
	char command[256];
	int status;

	remove(image);
	snprintf(command, sizeof(command), "../os9/os9 format -q %s %s > /dev/null 2>&1", arguments, image);
	status = system(command);
	ASSERT_EQUALS(0, status);
	*before_size = read_file(image, before, GAP_IMAGE_MAX);
}

/* The rest of a sector of a new end of file is $FF, but the sectors skipped
   to get there are zero, and a sector already in a gap keeps its zeros. */
void test_os9_raw_extend_of_quick_format()
{
	static u_char before[GAP_IMAGE_MAX], after[GAP_IMAGE_MAX];
	const char *image = "test_gap.dsk";
	u_char data[32];
	os9_path_id p;
	long before_size, after_size;
	error_code ec;

	format_gap_image("-t40", image, before, &before_size);
	ASSERT_EQUALS(11 * 256, before_size);
	memset(data, 0x5C, sizeof(data));

	ec = _os9_open(&p, "test_gap.dsk,@", FAM_READ | FAM_WRITE);
	ASSERT_EQUALS(0, ec);
	ec = _os9_lsn_put(p, 100, 16, data, sizeof(data));
	ASSERT_EQUALS(0, ec);
	ec = _os9_lsn_put(p, 50, 16, data, sizeof(data));
	ASSERT_EQUALS(0, ec);
	ec = _os9_lsn_put(p, 101, 16, data, sizeof(data));
	ASSERT_EQUALS(0, ec);
	ec = _os9_close(p);
	ASSERT_EQUALS(0, ec);

	after_size = read_file(image, after, sizeof(after));
	ASSERT_EQUALS(102 * 256, after_size);
	ASSERT_MEM_EQUALS(before, after, before_size);

	/* Past the old end, as far as the sector written back into the gap */
	ASSERT_EQUALS(0, count_other(after, before_size, 50 * 256 + 16, 0x00));
	ASSERT_EQUALS(0, count_other(after, 50 * 256 + 16, 50 * 256 + 48, 0x5C));
	ASSERT_EQUALS(0, count_other(after, 50 * 256 + 48, 100 * 256, 0x00));

	/* The last sectors, past a gap and with none */
	ASSERT_EQUALS(0, count_other(after, 100 * 256, 100 * 256 + 16, 0xFF));
	ASSERT_EQUALS(0, count_other(after, 100 * 256 + 16, 100 * 256 + 48, 0x5C));
	ASSERT_EQUALS(0, count_other(after, 100 * 256 + 48, 101 * 256 + 16, 0xFF));
	ASSERT_EQUALS(0, count_other(after, 101 * 256 + 16, 101 * 256 + 48, 0x5C));
	ASSERT_EQUALS(0, count_other(after, 101 * 256 + 48, 102 * 256, 0xFF));
	remove(image);
}

/* Neither file fills its last sector, to see what pads each of them. */
#define GAP_BOOT_SIZE	2100
#define GAP_TRACK_SIZE	4500
#define GAP_TRACK_LSN	(34 * 18)

static void write_gen_files(void)
{
	static char boot[GAP_BOOT_SIZE], track[GAP_TRACK_SIZE];
	FILE *fp;

	memset(boot, 0x87, sizeof(boot));
	memset(track, 0x4F, sizeof(track));
	fp = fopen("test_gap_boot.bin", "wb");
	fwrite(boot, 1, sizeof(boot), fp);
	fclose(fp);
	fp = fopen("test_gap_track.bin", "wb");
	fwrite(track, 1, sizeof(track), fp);
	fclose(fp);
}

static void run_gen(const char *image)
{
	char command[256];
	int status;

	snprintf(command, sizeof(command),
		 "../os9/os9 gen -b=test_gap_boot.bin -t=test_gap_track.bin %s > /dev/null 2>&1", image);
	status = system(command);
	ASSERT_EQUALS(0, status);
}

/* gen writes the kernel track before OS9Boot, so on a quick format it first
   extends the image to track 34. */
void test_os9_gen_on_quick_format()
{
	static u_char before[GAP_IMAGE_MAX], after[GAP_IMAGE_MAX];
	const char *image = "test_gap.dsk";
	const long track = GAP_TRACK_LSN * 256L;
	long before_size, after_size, boot, boot_end;
	int i;

	write_gen_files();
	format_gap_image("-t40", image, before, &before_size);
	run_gen(image);

	after_size = read_file(image, after, sizeof(after));
	ASSERT_EQUALS((GAP_TRACK_LSN + 18) * 256L, after_size);

	/* What was there is kept, but for the sectors gen has to change. */
	ASSERT_MEM_EQUALS(before + 0x1F, after + 0x1F, 256 - 0x1F);
	for (i = 0; i < 256; i++)
	{
		ASSERT_EQUALS(before[256 + i], after[256 + i] & before[256 + i]);
	}
	ASSERT_MEM_EQUALS(before + 3 * 256, after + 3 * 256, 64);
	ASSERT_EQUALS(0, count_other(after, 4 * 256, before_size, 0x00));

	/* OS9Boot lands in the gap, whose zeros stay after its last byte. */
	boot = int3(((lsn0_sect *) after)->dd_bt) * 256L;
	boot_end = boot + GAP_BOOT_SIZE;
	ASSERT_NEQUALS(0, boot);
	ASSERT_EQUALS(GAP_BOOT_SIZE, int2(((lsn0_sect *) after)->dd_bsz));
	ASSERT_EQUALS(0, count_other(after, boot, boot_end, 0x87));
	ASSERT_EQUALS(0, count_other(after, boot_end, track, 0x00));

	/* The kernel track is the end of the image, and $FF pads its last sector. */
	ASSERT_EQUALS(0, count_other(after, track, track + GAP_TRACK_SIZE, 0x4F));
	ASSERT_EQUALS(0, count_other(after, track + GAP_TRACK_SIZE, after_size, 0xFF));

	remove(image);
	remove("test_gap_boot.bin");
	remove("test_gap_track.bin");
}

/* A fully formatted image has no gap: gen changes only what it writes. */
void test_os9_gen_on_full_format()
{
	static u_char before[GAP_IMAGE_MAX], after[GAP_IMAGE_MAX];
	const char *image = "test_gap.dsk";
	const long track = GAP_TRACK_LSN * 256L;
	long before_size, after_size, boot, boot_end;

	write_gen_files();
	format_gap_image("-e -t40", image, before, &before_size);
	ASSERT_EQUALS(40 * 18 * 256L, before_size);
	run_gen(image);

	after_size = read_file(image, after, sizeof(after));
	ASSERT_EQUALS(before_size, after_size);

	/* Past the root directory, all but OS9Boot, its descriptor before it,
	   and the kernel track stays as formatted. */
	boot = int3(((lsn0_sect *) after)->dd_bt) * 256L;
	boot_end = boot + GAP_BOOT_SIZE;
	ASSERT_EQUALS(0, count_other(after, boot, boot_end, 0x87));
	ASSERT_MEM_EQUALS(before + 4 * 256, after + 4 * 256, boot - 256 - 4 * 256);
	ASSERT_MEM_EQUALS(before + boot_end, after + boot_end, track - boot_end);
	ASSERT_EQUALS(0, count_other(after, track, track + GAP_TRACK_SIZE, 0x4F));
	ASSERT_MEM_EQUALS(before + track + GAP_TRACK_SIZE, after + track + GAP_TRACK_SIZE,
			  after_size - track - GAP_TRACK_SIZE);

	remove(image);
	remove("test_gap_boot.bin");
	remove("test_gap_track.bin");
}

#define REAL_OS9 "\"../../../unittest/testdata/os9 files mame.dmk\""
#define REAL_OS9_OUTPUT "test_real_os9.txt"

/* Run a command on a disk that OS-9 made, keeping its output. */
static int real_os9_command(const char *arguments, char *output, size_t capacity)
{
	char command[256];
	FILE *fp;
	size_t got;
	int status;

	snprintf(command, sizeof(command), "../os9/os9 %s > " REAL_OS9_OUTPUT " 2>&1", arguments);
	status = system(command);
	output[0] = '\0';
	fp = fopen(REAL_OS9_OUTPUT, "rb");
	if (fp != NULL)
	{
		got = fread(output, 1, capacity - 1, fp);
		output[got] = '\0';
		fclose(fp);
	}
	remove(REAL_OS9_OUTPUT);

	return status;
}

/* Independently obtain data positions from the test containers. */
static void reveal_data_offset(const char *image, diskimage_format format,
			       unsigned int cylinder, unsigned int side, unsigned int sector, long *result)
{
	FILE *fp = fopen(image, "rb");
	long offset, record;
	u_char bytes[4];
	diskimage_id disk;
	diskimage_geometry geometry;

	ASSERT("open reveal image", fp != NULL);
	if (format == DISKIMAGE_DMK)
		offset = damage_dmk_id(fp, cylinder, side, sector) + 45;
	else if (format == DISKIMAGE_SDF)
	{
		offset = damage_sdf_entry(fp, cylinder, side, sector, &record);
		ASSERT("the reveal sector exists", offset >= 0);
		fseek(fp, offset + 2, SEEK_SET);
		fread(bytes, 1, 2, fp);
		offset = record + ((bytes[0] | (bytes[1] << 8)) & 0x3FFF);
	}
	else
	{
		ASSERT_EC(0, _diskimage_open(&disk, image, FAM_READ));
		ASSERT_EC(0, _diskimage_query_geometry(disk, &geometry));
		ASSERT_EC(0, _diskimage_close(disk));
		offset = ((cylinder * geometry.sides + side) * geometry.sectors_per_track + sector - 1) * geometry.sector_size;
		if (format == DISKIMAGE_VDK)
		{
			fread(bytes, 1, 4, fp);
			offset += bytes[2] | (bytes[3] << 8);
		}
		else if (format == DISKIMAGE_JVC)
		{
			fseek(fp, 0, SEEK_END);
			offset += ftell(fp) % 256;
		}
	}
	fclose(fp);
	*result = offset;
}

void test_os9_reveal_container_data()
{
	static const struct {
		const char *image;
		const char *options;
		diskimage_format format;
	} cases[] = {
		{ "test_reveal.dsk", "-e", DISKIMAGE_RAW },
		{ "test_reveal.dsk", "-e -sz10", DISKIMAGE_RAW },
		{ "test_reveal.vdk", "", DISKIMAGE_VDK },
		{ "test_reveal.jvc", "-ds -st10", DISKIMAGE_JVC },
		{ "test_reveal.dmk", "-ds", DISKIMAGE_DMK },
		{ "test_reveal.sdf", "", DISKIMAGE_SDF },
		{ "test_reveal.dmk", "-sz10", DISKIMAGE_DMK },
		{ "test_reveal.dmk", "-bs512 -st9", DISKIMAGE_DMK }
	};
	char arguments[192], output[4096], expected[4096], pathlist[80];
	os9_path_id path;
	unsigned int bps, root, total, targets[5], spt, t0s, sides;
	size_t i, j;
	int status;

	for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
	{
		remove(cases[i].image);
		snprintf(arguments, sizeof(arguments), "format -q %s %s", cases[i].options, cases[i].image);
		status = real_os9_command(arguments, output, sizeof(output));
		ASSERT_EQUALS(0, status);
		snprintf(pathlist, sizeof(pathlist), "%s,@", cases[i].image);
		ASSERT_EC(0, _os9_open(&path, pathlist, FAM_READ));
		bps = path->bps;
		root = int3(path->lsn0->dd_dir);
		total = int3(path->lsn0->dd_tot);
		spt = path->spt;
		t0s = path->t0s;
		sides = path->sides;
		ASSERT_EC(0, _os9_close(path));
		targets[0] = 0;
		targets[1] = 1;
		targets[2] = root;
		targets[3] = root + 1;
		targets[4] = total - 1;
		for (j = 0; j < sizeof(targets) / sizeof(targets[0]); j++)
		{
			unsigned int lsn = targets[j];
			unsigned int track = lsn < t0s ? 0 : (lsn - t0s) / spt + 1;
			unsigned int sector = lsn < t0s ? lsn + 1 : (lsn - t0s) % spt + 1;
			long offset = -1;
			char *description;

			reveal_data_offset(cases[i].image, cases[i].format, track / sides, track % sides, sector, &offset);
			ASSERT("sector data offset found", offset >= 0);

			snprintf(arguments, sizeof(arguments), "reveal %s %u:%u", cases[i].image, lsn, bps - 1);
			status = real_os9_command(arguments, expected, sizeof(expected));
			ASSERT_EQUALS(0, status);
			description = strstr(expected, "This is");
			if (description == NULL)
				description = strstr(expected, "doesn't currently map");
			ASSERT("filesystem description found", description != NULL);
			snprintf(arguments, sizeof(arguments), "reveal -b %s %ld", cases[i].image, offset + bps - 1);
			status = real_os9_command(arguments, output, sizeof(output));
			ASSERT_EQUALS(0, status);
			ASSERT("container data maps to the same filesystem location", strstr(output, description) != NULL);
		}
		remove(cases[i].image);
	}
}

void test_os9_reveal_container_fields()
{
	char arguments[128], output[4096];
	long first, second, next_track;
	int status;
	struct stat st;

	remove("test_reveal.dmk");
	status = real_os9_command("format -q test_reveal.dmk", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	reveal_data_offset("test_reveal.dmk", DISKIMAGE_DMK, 0, 0, 1, &first);
	reveal_data_offset("test_reveal.dmk", DISKIMAGE_DMK, 0, 0, 7, &second);
	status = real_os9_command("reveal -b test_reveal.dmk 0", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("image header", strstr(output, "image header") != NULL);
	ASSERT("not LSN0", strstr(output, "Examining LSN") == NULL);
	status = real_os9_command("reveal -b test_reveal.dmk 16", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("track header", strstr(output, "track header") != NULL);
	snprintf(arguments, sizeof(arguments), "reveal -b test_reveal.dmk %ld", first - 45);
	status = real_os9_command(arguments, output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("sector ID", strstr(output, "sector 1's ID field") != NULL);
	snprintf(arguments, sizeof(arguments), "reveal -b test_reveal.dmk %ld", first + 256);
	status = real_os9_command(arguments, output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("sector CRC", strstr(output, "sector 1's data CRC") != NULL);
	snprintf(arguments, sizeof(arguments), "reveal -b test_reveal.dmk %ld", first + 263);
	status = real_os9_command(arguments, output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("gap after physical sector", strstr(output, "6 bytes after the last byte of sector 1's data CRC") != NULL);
	snprintf(arguments, sizeof(arguments), "%ld bytes before sector 7's ID field", second - 45 - (first + 263));
	ASSERT("next physical sector, not next logical sector", strstr(output, arguments) != NULL);
	ASSERT("gap is not filesystem data", strstr(output, "Examining LSN") == NULL);

	/* Missing sectors do not shift LSNs, and their IDs remain describable. */
	reveal_data_offset("test_reveal.dmk", DISKIMAGE_DMK, 1, 0, 1, &next_track);
	image_remove_sector("test_reveal.dmk", 1, 0, 1);
	snprintf(arguments, sizeof(arguments), "reveal -b test_reveal.dmk %ld", next_track);
	status = real_os9_command(arguments, output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("rejected sector", strstr(output, "not a logical sector") != NULL);
	ASSERT("no filesystem interpretation", strstr(output, "Examining LSN") == NULL);
	reveal_data_offset("test_reveal.dmk", DISKIMAGE_DMK, 1, 0, 2, &next_track);
	snprintf(arguments, sizeof(arguments), "reveal -b test_reveal.dmk %ld", next_track);
	status = real_os9_command(arguments, output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("following LSN unchanged", strstr(output, "Examining LSN 19") != NULL);
	stat("test_reveal.dmk", &st);
	snprintf(arguments, sizeof(arguments), "reveal -b test_reveal.dmk %lld", (long long)st.st_size);
	status = real_os9_command(arguments, output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("file boundary", strstr(output, "past the physical end") != NULL);
	status = real_os9_command("reveal -b test_reveal.dmk 12junk", output, sizeof(output));
	ASSERT_NEQUALS(0, status);
	remove("test_reveal.dmk");

	/* A header can be explained without an OS-9 filesystem at all. */
	ASSERT_EC(0, _diskimage_create("test_reveal.vdk", DISKIMAGE_VDK, 1, 1, 256, 18, 18, 1, 0xFF));
	status = real_os9_command("reveal -b test_reveal.vdk 1", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("container alone", strstr(output, "image header") != NULL);
	remove("test_reveal.vdk");

	status = real_os9_command("format -q -e -sz10 test_reveal.dsk", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	status = real_os9_command("reveal -b test_reveal.dsk 2560", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("unused raw track-zero slot", strstr(output, "unused slot on short track zero") != NULL);
	ASSERT("unused slot is not an LSN", strstr(output, "Examining LSN") == NULL);
	status = real_os9_command("reveal test_reveal.dsk 0:256", output, sizeof(output));
	ASSERT_NEQUALS(0, status);
	remove("test_reveal.dsk");
}

static int count_lines(const char *text)
{
	int lines = 0;

	for (; *text != '\0'; text++)
	{
		lines += *text == '\n';
	}

	return lines;
}

/* What OS-9's free said, with the free sectors in many holes. */
void test_os9_free_of_real_disk()
{
	static char output[4096];
	int status = real_os9_command("free " REAL_OS9 ",", output, sizeof(output));

	ASSERT_EQUALS(0, status);
	ASSERT("as OS-9 said", strstr(output, "138 Free sectors, largest block 114 sectors\n") != NULL);
}

/* A file nobody can read does not end the listing. */
void test_os9_dir_extended_of_real_disk()
{
	static char output[16384];
	int status = real_os9_command("dir -e " REAL_OS9 ",", output, sizeof(output));

	ASSERT_EQUALS(0, status);
	/* The heading is 4 lines, and OS-9 listed 29 entries. */
	ASSERT_EQUALS(4 + 29, count_lines(output));
	ASSERT("no permissions", strstr(output, "  0.0     1987/02/14 1212   --------      BF       300 ATTRNONE\n") != NULL);
	ASSERT("the owner", strstr(output, "  1.2     1987/02/14 1222   ---wr-wr     1CB       100 OWNED\n") != NULL);
	ASSERT("the last entry", strstr(output, "1CD       875 DIRCMD\n") != NULL);

	status = real_os9_command("dir -e -r " REAL_OS9 ",", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("the deepest file", strstr(output, "11F       512 FILE\n") != NULL);
	ASSERT("the last of the big directory", strstr(output, "19F         0 F69\n") != NULL);
}

/* A file descriptor that cannot be read is reported, and the rest still list. */
void test_os9_dir_extended_missing_fd()
{
	static char output[16384];
	int status;

	status = system("cp " REAL_OS9 " test_real_os9.dmk");
	ASSERT_EQUALS(0, status);
	/* EMPTY's file descriptor is LSN 10. */
	image_remove_sector("test_real_os9.dmk", 0, 0, 11);
	status = real_os9_command("dir -e test_real_os9.dmk,", output, sizeof(output));
	ASSERT_NEQUALS(0, status);
	ASSERT("names the file", strstr(output, "error 241 reading the file descriptor of 'test_real_os9.dmk,./EMPTY'\n") != NULL);
	ASSERT("the next one is listed", strstr(output, "1 ONEBYTE\n") != NULL);
	ASSERT("the last one is listed", strstr(output, " DIRCMD\n") != NULL);
	remove("test_real_os9.dmk");
}

void test_os9_fstat_of_real_disk()
{
	static char output[4096];
	int status;

	status = real_os9_command("fstat " REAL_OS9 ",ATTRNONE", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("no permissions", strstr(output, "Attributes         : --------\n") != NULL);
	ASSERT("its size", strstr(output, "File size          : 300 ($12C)\n") != NULL);

	status = real_os9_command("fstat " REAL_OS9 ",OWNED", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("its owner", strstr(output, "Owner              : 1.2") != NULL);

	status = real_os9_command("fstat " REAL_OS9 ",BIGDIR", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("a directory", strstr(output, "Attributes         : d-ewrewr\n") != NULL);
	ASSERT("two segments", strstr(output, " 2. LSN2 (") != NULL && strstr(output, " 3. LSN3 (") == NULL);

	status = real_os9_command("fstat " REAL_OS9 ",FULLSEGS", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("as many segments as RBF allows", strstr(output, "47. LSN47 (") != NULL && strstr(output, "48. LSN48 (") == NULL);
}

/* OS-9 counts directories among the files, as 111, and this counts them apart. */
void test_os9_dcheck_of_real_disk()
{
	static char output[4096];
	int status = real_os9_command("dcheck " REAL_OS9, output, sizeof(output));

	ASSERT_EQUALS(0, status);
	ASSERT("intact", strstr(output, "'FILES' file structure is intact\n") != NULL);
	ASSERT("the directories", strstr(output, "10 directories\n") != NULL);
	ASSERT("the files", strstr(output, "102 files\n") != NULL);
}

void test_os9_ident_of_real_disk()
{
	static char output[4096];
	int status = real_os9_command("ident " REAL_OS9 ",DIRCMD", output, sizeof(output));

	ASSERT_EQUALS(0, status);
	ASSERT("the size", strstr(output, "$36B") != NULL);
	ASSERT("the CRC OS-9 reported", strstr(output, "$4D8369 (Good)") != NULL);
}

/* After changes, the disk still has what dcheck looks for. */
void test_os9_dcheck_after_changing_real_disk()
{
	static char output[4096];
	int status;

	status = system("cp " REAL_OS9 " test_real_os9.dmk");
	ASSERT_EQUALS(0, status);
	status = real_os9_command("del test_real_os9.dmk,FULLSEGS", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	status = real_os9_command("rename test_real_os9.dmk,S256 RENAMED", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	status = real_os9_command("makdir test_real_os9.dmk,DELS/NEWDIR", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	status = real_os9_command("deldir -q test_real_os9.dmk,ALLGONE", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	status = real_os9_command("dcheck test_real_os9.dmk", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("intact", strstr(output, "file structure is intact\n") != NULL);
	ASSERT("nothing lost", strstr(output, "0 clusters in file structure but not in allocation map\n") != NULL
	       && strstr(output, "0 clusters in allocation map but not in file structure\n") != NULL);
	remove("test_real_os9.dmk");
}

/*
 * OS-9 stops a file at 47 segments, but follows a 48th if a disk has one.  Tandy's
 * OS-9 read this one back, its dcheck said the file structure was intact, and
 * its free said 137 sectors with a largest block of 84.  It refused to add a
 * sector with E$SLF, and its del left the 48th segment allocated.
 */
#define FULL_LIST_IMAGE "test_48_segments.dmk"

static u_char pattern_byte(int seed, u_int offset)
{
	return (u_char) (seed + ((offset & 255) ^ (offset >> 8)));
}

/* Give FULLSEGS, of 47 one-sector segments and 12032 bytes, a 48th at LSN 600. */
static void add_48th_segment(void)
{
	u_char sector[256];
	os9_path_id p;
	fd_stats fd;
	int i, status;

	status = system("cp " REAL_OS9 " " FULL_LIST_IMAGE);
	ASSERT_EQUALS(0, status);
	ASSERT_EC(0, _os9_open(&p, FULL_LIST_IMAGE ",FULLSEGS", FAM_READ | FAM_WRITE));
	ASSERT_EC(0, _os9_gs_fd(p, sizeof(fd), &fd));
	ASSERT_EQUALS(0, int3(fd.fd_seg[47].lsn));
	for (i = 0; i < 256; i++)
	{
		sector[i] = pattern_byte(13, 12032 + i);
	}
	ASSERT_EC(0, _os9_lsn_put(p, 600, 0, sector, sizeof(sector)));
	_os9_allbit(p->bitmap, 600, 1);
	_int3(600, fd.fd_seg[47].lsn);
	_int2(1, fd.fd_seg[47].num);
	_int4(12288, fd.fd_siz);
	ASSERT_EC(0, _os9_lsn_put(p, p->pl_fd_lsn, 0, &fd, sizeof(fd)));
	ASSERT_EC(0, _os9_close(p));
}

void test_os9_48_segments_read()
{
	static u_char data[13000];
	static char output[4096];
	os9_path_id p;
	u_int size = sizeof(data), i;
	int status;

	add_48th_segment();
	ASSERT_EC(0, _os9_open(&p, FULL_LIST_IMAGE ",FULLSEGS", FAM_READ));
	ASSERT_EC(0, _os9_read(p, data, &size));
	ASSERT_EQUALS(12288, size);
	for (i = 0; i < size; i++)
	{
		ASSERT_EQUALS(pattern_byte(13, i), data[i]);
	}
	size = sizeof(data);
	ASSERT_EC(EOS_EOF, _os9_read(p, data, &size));
	ASSERT_EC(0, _os9_close(p));

	status = real_os9_command("free " FULL_LIST_IMAGE ",", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("as OS-9 said", strstr(output, "137 Free sectors, largest block 84 sectors\n") != NULL);
	remove(FULL_LIST_IMAGE);
}

void test_os9_48_segments_dcheck()
{
	static char output[4096];
	int status;

	add_48th_segment();
	status = real_os9_command("dcheck " FULL_LIST_IMAGE, output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("as OS-9 said", strstr(output, "'FILES' file structure is intact\n") != NULL);
	ASSERT("no bad descriptor", strstr(output, "0 bad file decriptor sector\n") != NULL);
	ASSERT("nothing lost", strstr(output, "0 clusters in allocation map but not in file structure\n") != NULL
	       && strstr(output, "0 clusters in file structure but not in allocation map\n") != NULL);
	remove(FULL_LIST_IMAGE);
}

/* The same in the loop that walks a directory: EMPTYDIR with 47 more segments. */
void test_os9_48_segments_directory_dcheck()
{
	static u_char sector[256];
	static char output[4096];
	os9_path_id p;
	fd_stats fd;
	int first, count, i, status;

	status = system("cp " REAL_OS9 " " FULL_LIST_IMAGE);
	ASSERT_EQUALS(0, status);
	ASSERT_EC(0, _os9_open(&p, FULL_LIST_IMAGE ",EMPTYDIR", FAM_DIR | FAM_READ | FAM_WRITE));
	ASSERT_EC(0, _os9_gs_fd(p, sizeof(fd), &fd));
	first = int3(fd.fd_seg[0].lsn);
	count = int2(fd.fd_seg[0].num);
	ASSERT_EQUALS(0, int3(fd.fd_seg[1].lsn));
	for (i = 1; i < count; i++)
	{
		ASSERT_EC(0, _os9_lsn_put(p, first + i, 0, sector, sizeof(sector)));
	}
	for (i = 1; i < NUM_SEGS; i++)
	{
		int lsn = 516 + (i - 1) * 2;

		ASSERT_EC(0, _os9_lsn_put(p, lsn, 0, sector, sizeof(sector)));
		_os9_allbit(p->bitmap, lsn, 1);
		_int3(lsn, fd.fd_seg[i].lsn);
		_int2(1, fd.fd_seg[i].num);
	}
	_int4((count + NUM_SEGS - 1) * 256, fd.fd_siz);
	ASSERT_EC(0, _os9_lsn_put(p, p->pl_fd_lsn, 0, &fd, sizeof(fd)));
	ASSERT_EC(0, _os9_close(p));

	status = real_os9_command("dcheck " FULL_LIST_IMAGE, output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("intact", strstr(output, "'FILES' file structure is intact\n") != NULL);
	ASSERT("no bad descriptor", strstr(output, "0 bad file decriptor sector\n") != NULL);
	ASSERT("nothing lost", strstr(output, "0 clusters in allocation map but not in file structure\n") != NULL
	       && strstr(output, "0 clusters in file structure but not in allocation map\n") != NULL);
	remove(FULL_LIST_IMAGE);
}

/* Like OS-9, a full list refuses another sector, and the file stays as it was. */
void test_os9_48_segments_cannot_grow()
{
	static u_char more[256], data[13000];
	os9_path_id p;
	u_int size = sizeof(more), i;

	add_48th_segment();
	ASSERT_EC(0, _os9_open(&p, FULL_LIST_IMAGE ",FULLSEGS", FAM_READ | FAM_WRITE));
	ASSERT_EC(0, _os9_seek(p, 12288, SEEK_SET));
	ASSERT_EC(EOS_SF, _os9_write(p, more, &size));
	ASSERT_EC(0, _os9_close(p));

	ASSERT_EC(0, _os9_open(&p, FULL_LIST_IMAGE ",FULLSEGS", FAM_READ));
	size = sizeof(data);
	ASSERT_EC(0, _os9_read(p, data, &size));
	ASSERT_EC(0, _os9_close(p));
	ASSERT_EQUALS(12288, size);
	for (i = 0; i < size; i++)
	{
		ASSERT_EQUALS(pattern_byte(13, i), data[i]);
	}
	remove(FULL_LIST_IMAGE);
}

/* OS-9's del leaves the 48th segment allocated, at 185 free.  This frees it. */
void test_os9_48_segments_delete()
{
	static char output[4096];
	int status;

	add_48th_segment();
	status = real_os9_command("del " FULL_LIST_IMAGE ",FULLSEGS", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	status = real_os9_command("free " FULL_LIST_IMAGE ",", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("all 48 and the file descriptor", strstr(output, "186 Free sectors, largest block 114 sectors\n") != NULL);
	remove(FULL_LIST_IMAGE);
}

#define REAL_CLUSTERS "\"../../../unittest/testdata/os9 clusters mame.dmk\""

/* What OS-9's free and dcheck said of a disk with 2-sector clusters. */
void test_os9_real_clusters()
{
	static char output[8192];
	int status;

	status = real_os9_command("free " REAL_CLUSTERS ",", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("as OS-9 said", strstr(output, "226 Free sectors, largest block 214 sectors\n") != NULL);
	ASSERT("the clusters", strstr(output, "2-sector clusters") != NULL);

	/* OS-9 counts the 3 directories under the root among its 27 files. */
	status = real_os9_command("dcheck " REAL_CLUSTERS, output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("intact", strstr(output, "'CLUSTER2' file structure is intact\n") != NULL);
	ASSERT("the directories", strstr(output, "4 directories\n") != NULL);
	ASSERT("the files", strstr(output, "24 files\n") != NULL);

	status = real_os9_command("dir -e " REAL_CLUSTERS ",", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("a segment that starts inside a cluster", strstr(output, "36      3000 UNCUT\n") != NULL);
}

/* The steps OS-9 took, and what its free said after each, leave nothing lost. */
void test_os9_real_clusters_delete()
{
	static const struct
	{
		const char *name;
		const char *free;
	} steps[] = {
		{ "EMPTY", "228 Free sectors, largest block 214 sectors\n" },
		{ "UNCUT", "242 Free sectors, largest block 214 sectors\n" },
		{ "COMB", "268 Free sectors, largest block 214 sectors\n" },
		{ "ODDA", "274 Free sectors, largest block 214 sectors\n" },
		{ "CUT1100", "280 Free sectors, largest block 214 sectors\n" },
	};
	static char output[8192];
	char arguments[128];
	size_t i;
	int status;

	status = system("cp " REAL_CLUSTERS " test_real_clusters.dmk");
	ASSERT_EQUALS(0, status);
	for (i = 0; i < sizeof(steps) / sizeof(steps[0]); i++)
	{
		snprintf(arguments, sizeof(arguments), "del test_real_clusters.dmk,%s", steps[i].name);
		status = real_os9_command(arguments, output, sizeof(output));
		ASSERT_EQUALS(0, status);
		status = real_os9_command("free test_real_clusters.dmk,", output, sizeof(output));
		ASSERT_EQUALS(0, status);
		ASSERT("as OS-9 said", strstr(output, steps[i].free) != NULL);
	}
	status = real_os9_command("dcheck test_real_clusters.dmk", output, sizeof(output));
	ASSERT_EQUALS(0, status);
	ASSERT("nothing lost", strstr(output, "0 clusters in allocation map but not in file structure\n") != NULL);
	remove("test_real_clusters.dmk");
}

int main()
{
	remove("test.dsk");
	RUN(test_os9_command_format);
	RUN(test_os9_command_copy_metadata);
	RUN(test_os9_command_copy_exit_status);
	RUN(test_os9_command_format_rejects_bad_t0s);
	RUN(test_os9_command_root_past_short_t0);
	RUN(test_os9_command_no_comma_spec);
	RUN(test_os9_command_copy_raw_path);
	RUN(test_os9_command_copy_eol_same_type);

	RUN(test_os9_command_copy_refuses_disk_images);
	RUN(test_os9_command_dsave_refuses_disk_images);
	RUN(test_os9_command_dsave_executable);
	RUN(test_os9_command_format_formats);
	RUN(test_os9_commands_on_images_with_tracks);
	RUN(test_os9_reveal_container_data);
	RUN(test_os9_reveal_container_fields);
	RUN(test_os9_gen_on_images_with_tracks);
	RUN(test_os9_raw_extend_of_quick_format);
	RUN(test_os9_gen_on_quick_format);
	RUN(test_os9_gen_on_full_format);
	RUN(test_os9_free_of_real_disk);
	RUN(test_os9_dir_extended_of_real_disk);
	RUN(test_os9_dir_extended_missing_fd);
	RUN(test_os9_fstat_of_real_disk);
	RUN(test_os9_dcheck_of_real_disk);
	RUN(test_os9_ident_of_real_disk);
	RUN(test_os9_dcheck_after_changing_real_disk);
	RUN(test_os9_48_segments_read);
	RUN(test_os9_48_segments_dcheck);
	RUN(test_os9_48_segments_directory_dcheck);
	RUN(test_os9_48_segments_cannot_grow);
	RUN(test_os9_48_segments_delete);
	RUN(test_os9_real_clusters);
	RUN(test_os9_real_clusters_delete);

	/* Fork CLI & regression tests */
	cleanup_test_files();
	RUN(test_os9_command_help_and_unknown);
	RUN(test_os9_command_line_parameters);
	RUN(test_os9_command_rdump_truncated_file_does_not_hang);
	RUN(test_os9_command_copy_rejects_double_comma_path);
	RUN(test_os9_command_format_variants);
	RUN(test_os9_command_copy_basic_roundtrip);
	RUN(test_os9_command_copy_to_directory);
	RUN(test_os9_command_copy_rewrite_option);
	RUN(test_os9_command_copy_eol_translation);
	RUN(test_os9_command_copy_buffer_and_owner);
	RUN(test_os9_command_copy_error_cases);
	RUN(test_os9_command_copy_disk_full);
	RUN(test_os9_command_copy_multi_file_failure);
	RUN(test_os9_command_dir);
	RUN(test_os9_command_id_and_free);
	RUN(test_os9_command_list_and_dump);
	RUN(test_os9_command_fstat);
	RUN(test_os9_command_makdir_and_deldir);
	RUN(test_os9_command_rename);
	RUN(test_os9_command_del);
	RUN(test_os9_command_attr);
	RUN(test_os9_command_cmp);
	RUN(test_os9_command_dcheck);
	RUN(test_os9_command_padrom);
	RUN(test_os9_command_gen);
	RUN(test_os9_gen_no_bitmap_leak);
	RUN(test_os9_integrity_stress);
	RUN(test_os9_command_ident);
	RUN(test_os9_cluster_allocation_and_deletion);
	cleanup_test_files();

	return TEST_REPORT();
}
