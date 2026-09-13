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

int main() {
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

  return TEST_REPORT();
}
