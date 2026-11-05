/*
 * Test EOL conversions used by os9 copy -l
 */

#include "tinytest.h"
#include <toolshed.h>

/* Workaround for nested test functions, needed because the tinytest
 * ASSERT will simply set error variables and return */
#define REPORT(name)                                                           \
  do {                                                                         \
    if (tt_current_test_failed) {                                              \
      printf("failure: While checking \"%s\":\n", name);                       \
      return;                                                                  \
    }                                                                          \
  } while (0);

/* Adjust size to break CR-LF across two chunks */
/* Note 5 will still break */
#define TEST_CHUNKSIZE 11
#define TEST_CRLF_DATA "1234\r\n7890\r\n3456\r\n"
#define TEST_LF_DATA "1234\n7890\n3456\n"
#define TEST_COCO_DATA "1234\r7890\r3456\r"

#define TRANSLATE_TO_DISK 1
#define TRANSLATE_FROM_DISK 2
void copy_to_disk_and_back(char *original, int translation, char *expected);
void create_disk(char *name);
void test_padrom();

/*
 * Native2Coco test method:
 * - create native LF file
 * - copy to disk image with translation
 * - copy verbatim back to native file
 * - compare with CR format
 *
 * repeat with native CRLF file
 *
 * Coco2Native test method:
 * - create CR file
 * - copy verbatim to disk image
 * - copy back with translation
 * - compare with native format (CRLF or LF)
 *
 */

void test_via_disk_image() {
  create_disk("test.dsk");
  REPORT("create_disk");

  copy_to_disk_and_back(TEST_LF_DATA, TRANSLATE_TO_DISK, TEST_COCO_DATA);
  REPORT("LF -> Coco");

  copy_to_disk_and_back(TEST_CRLF_DATA, TRANSLATE_TO_DISK, TEST_COCO_DATA);
  REPORT("CRLF -> Coco");

#ifdef WIN32
  copy_to_disk_and_back(TEST_COCO_DATA, TRANSLATE_FROM_DISK, TEST_CRLF_DATA);
#else
  copy_to_disk_and_back(TEST_COCO_DATA, TRANSLATE_FROM_DISK, TEST_LF_DATA);
#endif
  REPORT("Coco -> native");

  remove("test.dsk");
  remove("test-1.dat");
  remove("test-2.dat");
}

void copy_to_disk_and_back(char *original, int translation, char *expected) {
  FILE *f;
  int eolTranslate;
  int written;
  int was_read;
  int ec;
  char returned[256];
  char buffer[TEST_CHUNKSIZE];

  /* write original test content to local file */
  f = fopen("test-1.dat", "wb");
  ASSERT("fopen", f != NULL);
  written = fwrite(original, 1, strlen(original), f);
  fclose(f);
  ASSERT_EQUALS(strlen(original), written);

  /* copy to disk image */
  eolTranslate = (translation == TRANSLATE_TO_DISK);
  ec = TSCopyFile("test-1.dat", "test.dsk,test.dat", eolTranslate, 1, 0, 0,
                  buffer, TEST_CHUNKSIZE);
  ASSERT_EQUALS(0, ec);

  /* copy back */
  eolTranslate = (translation == TRANSLATE_FROM_DISK);
  ec = TSCopyFile("test.dsk,test.dat", "test-2.dat", eolTranslate, 1, 0, 0,
                  buffer, TEST_CHUNKSIZE);
  ASSERT_EQUALS(0, ec);

  /* read content from new local file */
  f = fopen("test-2.dat", "rb");
  was_read = fread(returned, 1, sizeof(returned), f);
  fclose(f);
  ASSERT("Failed fread", was_read > 0);

  if (was_read > 0)
    returned[was_read] = '\0';
  else
    returned[0] = '\0';

  ASSERT_STRING_EQUALS(expected, returned);
}

/* only a helper, disk creation is properly tested in librbf tests */
void create_disk(char *name) {
  error_code ec;

  /* Create disk */
  unsigned int totalSectors, totalBytes;
  int clusterSize = 0;
  ec = _os9_format(name, 0, 80, 18, 18, 2, 256, &clusterSize, "Test LTS", 8, 8,
                   1, 1, 0, 0, 3, &totalSectors, &totalBytes);
  ASSERT_EQUALS(0, ec);
}

/* simple test of EOL auto-detection on a single chunk */
void test_native2coco() {
  char *newBuffer;
  u_int newSize;

  NativeToCoCo(TEST_LF_DATA, strlen(TEST_LF_DATA), &newBuffer, &newSize);
  ASSERT_EQUALS(strlen(TEST_COCO_DATA), newSize);
  ASSERT_MEM_EQUALS(TEST_COCO_DATA, newBuffer, newSize);

  free(newBuffer);

  NativeToCoCo(TEST_CRLF_DATA, strlen(TEST_CRLF_DATA), &newBuffer, &newSize);
  ASSERT_EQUALS(strlen(TEST_COCO_DATA), newSize);
  ASSERT_MEM_EQUALS(TEST_COCO_DATA, newBuffer, newSize);

  free(newBuffer);
}

/* unfortunately only tests for current native platform */
void test_coco2native() {
  char *newBuffer;
  u_int newSize;

  CoCoToNative(TEST_COCO_DATA, strlen(TEST_COCO_DATA), &newBuffer, &newSize);
#ifdef WIN32
  ASSERT_EQUALS(strlen(TEST_CRLF_DATA), newSize);
  ASSERT_MEM_EQUALS(TEST_CRLF_DATA, newBuffer, newSize);
#else
  ASSERT_EQUALS(strlen(TEST_LF_DATA), newSize);
  ASSERT_MEM_EQUALS(TEST_LF_DATA, newBuffer, newSize);
#endif
  free(newBuffer);
}

void test_copy_edge_cases() {
  char buffer[TEST_CHUNKSIZE];
  int ec;

  /* copy to invalid path */
  ec = TSCopyFile("nonexistent.dat", "test.dsk,test.dat", 0, 1, 0, 0, buffer,
                  TEST_CHUNKSIZE);
  ASSERT_NEQUALS(0, ec);

  /* copy from nonexistent file */
  ec = TSCopyFile("test.dsk,nonexistent.dat", "test-out.dat", 0, 1, 0, 0,
                  buffer, TEST_CHUNKSIZE);
  ASSERT_NEQUALS(0, ec);
}

void test_padrom() {
  FILE *f;
  error_code ec;
  char buffer[16];
  u_int size;

  /* Create a small file "test-pad.dat" with 4 bytes */
  f = fopen("test-pad.dat", "wb");
  ASSERT("fopen", f != NULL);
  fwrite("1234", 1, 4, f);
  fclose(f);

  /* Pad to 10 bytes with 'X' at the end */
  ec = TSPadROM("test-pad.dat", 10, 'X', 0);
  ASSERT_EQUALS(0, ec);

  /* Verify size */
  f = fopen("test-pad.dat", "rb");
  ASSERT("fopen", f != NULL);
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  ASSERT_EQUALS(10, size);

  /* Verify content */
  fseek(f, 0, SEEK_SET);
  fread(buffer, 1, 10, f);
  fclose(f);
  buffer[10] = '\0';
  ASSERT_STRING_EQUALS("1234XXXXXX", buffer);

  /* Clean up */
  remove("test-pad.dat");

  /* Test padding at start */
  f = fopen("test-pad.dat", "wb");
  fwrite("1234", 1, 4, f);
  fclose(f);

  /* Pad to 8 bytes with 'Y' at the start */
  ec = TSPadROM("test-pad.dat", 8, 'Y', 1);
  ASSERT_EQUALS(0, ec);

  /* Verify content */
  f = fopen("test-pad.dat", "rb");
  fread(buffer, 1, 8, f);
  fclose(f);
  buffer[8] = '\0';
  ASSERT_STRING_EQUALS("YYYY1234", buffer);
  remove("test-pad.dat");

  /* Test large padding across 4096-byte chunk boundary (e.g. 10000 bytes) */
  f = fopen("test-pad.dat", "wb");
  fwrite("1234", 1, 4, f);
  fclose(f);

  ec = TSPadROM("test-pad.dat", 10000, 'Z', 0);
  ASSERT_EQUALS(0, ec);

  f = fopen("test-pad.dat", "rb");
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fclose(f);
  ASSERT_EQUALS(10000, size);
  remove("test-pad.dat");

  /* Large padding at start */
  f = fopen("test-pad.dat", "wb");
  fwrite("1234", 1, 4, f);
  fclose(f);

  ec = TSPadROM("test-pad.dat", 10000, 'W', 1);
  ASSERT_EQUALS(0, ec);

  f = fopen("test-pad.dat", "rb");
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fclose(f);
  ASSERT_EQUALS(10000, size);
  remove("test-pad.dat");
}

void test_native_readln() {
  FILE *f = fopen("test_readln.txt", "wb");
  ASSERT("fopen", f != NULL);
  /* Write:
   * Line 1: "Hello\n" (6 bytes)
   * Line 2: "\n" (1 byte empty line)
   * Line 3: "World" (5 bytes, no trailing newline)
   */
  const char *content = "Hello\n\nWorld";
  fwrite(content, 1, strlen(content), f);
  fclose(f);

  native_path_id path;
  error_code ec = _native_open(&path, "test_readln.txt", FAM_READ);
  ASSERT_EQUALS(0, ec);

  char buf[64];
  u_int size;

  /* Line 1: "Hello\n" -> translated to "Hello\r", size 6 */
  memset(buf, 0, sizeof(buf));
  size = sizeof(buf);
  ec = _native_readln(path, buf, &size);
  ASSERT_EQUALS(0, ec);
  ASSERT_EQUALS(6, size);
  ASSERT_EQUALS(0x0D, (unsigned char)buf[5]);
  ASSERT_EQUALS(0, memcmp(buf, "Hello\r", 6));

  /* Line 2: "\n" (empty line) -> translated to "\r", size 1 */
  memset(buf, 0, sizeof(buf));
  size = sizeof(buf);
  ec = _native_readln(path, buf, &size);
  ASSERT_EQUALS(0, ec);
  ASSERT_EQUALS(1, size);
  ASSERT_EQUALS(0x0D, (unsigned char)buf[0]);

  /* Line 3: "World" -> at EOF without newline, size 5 */
  memset(buf, 0, sizeof(buf));
  size = sizeof(buf);
  ec = _native_readln(path, buf, &size);
  ASSERT_EQUALS(0, ec);
  ASSERT_EQUALS(5, size);
  ASSERT_EQUALS(0, memcmp(buf, "World", 5));

  /* Line 4: at EOF -> EOS_EOF */
  size = sizeof(buf);
  ec = _native_readln(path, buf, &size);
  ASSERT_EQUALS(EOS_EOF, ec);

  ec = _native_close(path);
  ASSERT_EQUALS(0, ec);

  remove("test_readln.txt");
}

#define FS_OS9_MAME "../../../unittest/testdata/os9 mame.dmk"
#define FS_DRAGON_MAME "../../../unittest/testdata/dragondos mame.vdk"
#define FS_IMAGE "test_filesystems.dsk"

static u_int detected(const char *pathlist)
{
	u_int filesystems = 0xFF;

	return _coco_detect_filesystems(pathlist, &filesystems) == 0 ? filesystems : 0xFF;
}

/* Put an empty Disk BASIC FAT and directory on track 17 of an image. */
static void add_decb_track(const char *pathname)
{
	diskimage_id disk;
	u_char sector[256];
	error_code ec = _diskimage_open(&disk, pathname, FAM_READ | FAM_WRITE);
	int number;

	ASSERT_EQUALS(0, ec);
	memset(sector, 0, sizeof(sector));
	memset(sector, 0xFF, 68);
	ec = _diskimage_write_sector(disk, 17, 0, 2, sector, sizeof(sector));
	ASSERT_EQUALS(0, ec);
	memset(sector, 0xFF, sizeof(sector));
	for (number = 3; number <= 11; number++)
	{
		ec = _diskimage_write_sector(disk, 17, 0, number, sector, sizeof(sector));
		ASSERT_EQUALS(0, ec);
	}
	_diskimage_close(disk);
}

void test_detect_filesystems()
{
	char names[64];
	unsigned int total_sectors, total_bytes;
	int cluster_size = 0;
	decb_path_id path;
	u_int count = 5;
	error_code ec;
	u_int found;

	/* Real disks */
	found = detected(FS_OS9_MAME);
	ASSERT_EQUALS(COCO_FS_OS9, found);
	found = detected(FS_DRAGON_MAME);
	ASSERT_EQUALS(COCO_FS_DRAGONDOS, found);

	/* A pathlist means its image. */
	found = detected(FS_OS9_MAME ",SOME/FILE");
	ASSERT_EQUALS(COCO_FS_OS9, found);

	/* An empty Disk BASIC disk, then with a file */
	remove(FS_IMAGE);
	ec = _decb_dskini(FS_IMAGE, 35, NULL, 1, 256, 0);
	ASSERT_EQUALS(0, ec);
	found = detected(FS_IMAGE);
	ASSERT_EQUALS(COCO_FS_DECB, found);
	ec = _decb_create(&path, FS_IMAGE ",FILE.TXT", FAM_WRITE, 0, 0);
	ASSERT_EQUALS(0, ec);
	ec = _decb_write(path, "hello", &count);
	ASSERT_EQUALS(0, ec);
	ec = _decb_close(path);
	ASSERT_EQUALS(0, ec);
	found = detected(FS_IMAGE);
	ASSERT_EQUALS(COCO_FS_DECB, found);

	/* A bad chain in its FAT means it is not one. */
	{
		FILE *fp = fopen(FS_IMAGE, "r+b");
		u_char bad = 0xD0;
		int i;

		for (i = 0; i < 68; i++)
		{
			u_char byte;

			fseek(fp, (17 * 18 + 1) * 256 + i, SEEK_SET);
			fread(&byte, 1, 1, fp);
			if (byte != 0xFF)
			{
				fseek(fp, (17 * 18 + 1) * 256 + i, SEEK_SET);
				fwrite(&bad, 1, 1, fp);
				break;
			}
		}
		fclose(fp);
	}
	found = detected(FS_IMAGE);
	ASSERT_EQUALS(0, found);

	/* A half OS-9 half Disk BASIC disk made by decb dskini -s */
	remove(FS_IMAGE);
	ec = _decb_dskini(FS_IMAGE, 35, NULL, 1, 256, 1);
	ASSERT_EQUALS(0, ec);
	found = detected(FS_IMAGE);
	ASSERT_EQUALS(COCO_FS_DECB, found);

	/* OS-9, which does not touch track 17 in a way Disk BASIC could use */
	remove(FS_IMAGE);
	ec = _os9_format(FS_IMAGE, 0, 35, 18, 18, 1, 256, &cluster_size, "Test", 8, 8, 1, 1, 0, 0, 3,
			 &total_sectors, &total_bytes);
	ASSERT_EQUALS(0, ec);
	found = detected(FS_IMAGE);
	ASSERT_EQUALS(COCO_FS_OS9, found);

	/* Both at once */
	add_decb_track(FS_IMAGE);
	found = detected(FS_IMAGE);
	ASSERT_EQUALS(COCO_FS_OS9 | COCO_FS_DECB, found);

	_coco_filesystem_names(COCO_FS_OS9 | COCO_FS_DECB, names, sizeof(names));
	ASSERT_STRING_EQUALS("OS-9, Disk BASIC", names);
	_coco_filesystem_names(COCO_FS_OS9 | COCO_FS_DRAGONDOS, names, sizeof(names));
	ASSERT_STRING_EQUALS("OS-9, Dragon DOS", names);
	_coco_filesystem_names(COCO_FS_DECB | COCO_FS_DRAGONDOS, names, sizeof(names));
	ASSERT_STRING_EQUALS("Disk BASIC, Dragon DOS", names);
	_coco_filesystem_names(COCO_FS_DRAGONDOS, names, sizeof(names));
	ASSERT_STRING_EQUALS("Dragon DOS", names);
	remove(FS_IMAGE);

	/* An image with nothing on it, and a file that is not an image */
	ec = _diskimage_create(FS_IMAGE, DISKIMAGE_RAW, 1, 35, 256, 18, 0, 0, 0xFF);
	ASSERT_EQUALS(0, ec);
	found = detected(FS_IMAGE);
	ASSERT_EQUALS(0, found);
	remove(FS_IMAGE);
	{
		FILE *fp = fopen(FS_IMAGE, "wb");

		fputs("not a disk image", fp);
		fclose(fp);
	}
	found = detected(FS_IMAGE);
	ASSERT_EQUALS(0, found);
	remove(FS_IMAGE);

	/* A file that does not exist */
	ec = _coco_detect_filesystems("test_no_such_image.dsk", &found);
	ASSERT_EQUALS(EOS_PNNF, ec);
	ASSERT_EQUALS(0, found);
}

/* Each dir names the other filesystems, but not its own. */
void test_dir_lists_other_filesystems()
{
	static char output[8192];
	unsigned int total_sectors, total_bytes;
	int cluster_size = 0;
	size_t got;
	FILE *fp;
	int status;
	error_code ec;

	remove(FS_IMAGE);
	ec = _os9_format(FS_IMAGE, 0, 35, 18, 18, 1, 256, &cluster_size, "Test", 8, 8, 1, 1, 0, 0, 3,
			 &total_sectors, &total_bytes);
	ASSERT_EQUALS(0, ec);

	status = system("../os9/os9 dir " FS_IMAGE ", > test_fs_out.txt 2>&1");
	ASSERT_EQUALS(0, status);
	fp = fopen("test_fs_out.txt", "rb");
	got = fread(output, 1, sizeof(output) - 1, fp);
	fclose(fp);
	output[got] = 0;
	ASSERT("alone on the disk, nothing else is named", strstr(output, "Other possible") == NULL);

	add_decb_track(FS_IMAGE);

	status = system("../os9/os9 dir " FS_IMAGE ", > test_fs_out.txt 2>&1");
	ASSERT_EQUALS(0, status);
	fp = fopen("test_fs_out.txt", "rb");
	got = fread(output, 1, sizeof(output) - 1, fp);
	fclose(fp);
	output[got] = 0;
	ASSERT("os9 dir names Disk BASIC", strstr(output, "Other possible filesystems: Disk BASIC\n") != NULL);

	status = system("../decb/decb dir " FS_IMAGE ", > test_fs_out.txt 2>&1");
	ASSERT_EQUALS(0, status);
	fp = fopen("test_fs_out.txt", "rb");
	got = fread(output, 1, sizeof(output) - 1, fp);
	fclose(fp);
	output[got] = 0;
	ASSERT("decb dir names OS-9", strstr(output, "Other possible filesystems: OS-9\n") != NULL);

	remove("test_fs_out.txt");
	remove(FS_IMAGE);
}

int main()
{
  remove("test.dsk");
  remove("test-1.dat");
  remove("test-2.dat");
  remove("test-out.dat");
  remove("test_readln.txt");

  RUN(test_native2coco);
  RUN(test_coco2native);
  RUN(test_via_disk_image);
  RUN(test_copy_edge_cases);
  RUN(test_padrom);
  RUN(test_native_readln);

  remove("test.dsk");
  remove("test-1.dat");
  remove("test-2.dat");
  remove("test-out.dat");
  remove("test_readln.txt");

  RUN(test_detect_filesystems);
  RUN(test_dir_lists_other_filesystems);
  return TEST_REPORT();
}
