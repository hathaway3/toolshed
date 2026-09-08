/*
 * ALWAYS BE TESTING!!!
 *
 * ALWAYS BE WRITING MORE TESTS!!!
 *
 * THIS WORK IS **NEVER** DONE!!!
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

void test_os9_command_format()
{
	error_code ec;
	native_path_id nativepath;
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
	ASSERT_EQUALS(0, ec);
	
	ec = _os9_makdir("test.dsk,/CMDS");
	ASSERT_EQUALS(0, ec);
}

void test_os9_command_copy_disk_full()
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

void test_os9_command_copy_multi_file_failure()
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

int main()
{
	remove("test.dsk");
	remove("test_cli.dsk");
	remove("cli_big.tmp");
	remove("cli_small.tmp");

	RUN(test_os9_command_format);
	RUN(test_os9_command_copy_disk_full);
	RUN(test_os9_command_copy_multi_file_failure);

	remove("test.dsk");
	remove("test_cli.dsk");
	remove("cli_big.tmp");
	remove("cli_small.tmp");

	return TEST_REPORT();
}
