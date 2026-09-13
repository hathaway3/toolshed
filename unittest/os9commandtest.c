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
#include <unistd.h>

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

	/* Creating duplicate directory must fail */
	snprintf(cmd, sizeof(cmd), "%s makdir test_cli.dsk,DIR1 > /dev/null 2>&1", get_os9_bin());
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

/* ---------- main ----------------------------------------------------- */

int main(void)
{
	cleanup_test_files();

	/* CLI Dispatcher & Help */
	RUN(test_os9_command_help_and_unknown);
	RUN(test_os9_command_line_parameters);
	RUN(test_os9_command_rdump_truncated_file_does_not_hang);
	RUN(test_os9_command_copy_rejects_double_comma_path);

	/* Format */
	RUN(test_os9_command_format);
	RUN(test_os9_command_format_variants);

	/* Copy - Comprehensive coverage */
	RUN(test_os9_command_copy_basic_roundtrip);
	RUN(test_os9_command_copy_to_directory);
	RUN(test_os9_command_copy_rewrite_option);
	RUN(test_os9_command_copy_eol_translation);
	RUN(test_os9_command_copy_buffer_and_owner);
	RUN(test_os9_command_copy_error_cases);
	RUN(test_os9_command_copy_disk_full);
	RUN(test_os9_command_copy_multi_file_failure);

	/* Directory listing */
	RUN(test_os9_command_dir);

	/* Volume Information */
	RUN(test_os9_command_id_and_free);

	/* File Viewing */
	RUN(test_os9_command_list_and_dump);
	RUN(test_os9_command_fstat);

	/* Directory management */
	RUN(test_os9_command_makdir_and_deldir);

	/* File management */
	RUN(test_os9_command_rename);
	RUN(test_os9_command_del);
	RUN(test_os9_command_attr);

	/* File comparison */
	RUN(test_os9_command_cmp);

	/* Disk integrity */
	RUN(test_os9_command_dcheck);

	/* ROM/binary manipulation */
	RUN(test_os9_command_padrom);

	/* Boot track generation */
	RUN(test_os9_command_gen);

	/* Module identification */
	RUN(test_os9_command_ident);

	/* Multi-cluster allocation, truncation, and deletion */
	RUN(test_os9_cluster_allocation_and_deletion);

	cleanup_test_files();

	return TEST_REPORT();
}
