/*
 * ALWAYS BE TESTING!!!
 *
 * ALWAYS BE WRITING MORE TESTS!!!
 *
 * THIS WORK IS **NEVER** DONE!!!
 */

#include "tinytest.h"
#include <toolshed.h>

void test_decb_dskini() {
  error_code ec;

  // test format of a non-existent disk image
  ec = _decb_dskini("test.dsk", 35, "TEST", 1, 256, 0);
  ASSERT_EQUALS(0, ec);

  // test format of an existing disk image with a disk name that is way too long
  ec = _decb_dskini("test.dsk", 35,
                    "TESTDISKNAMEISWAYTOOLONGFORNORMALOPERATION", 1, 256, 0);
  ASSERT_EQUALS(EOS_BPNAM, ec);

  // test format of an existing disk image with a disk name that is barely too
  // long
  ec = _decb_dskini("test.dsk", 35, "TADTTOOLONG", 1, 256, 0);
  ASSERT_EQUALS(EOS_BPNAM, ec);

  // test format of an existing disk image with a disk name that is the max
  // length
  ec = _decb_dskini("test.dsk", 35, "JUSTRIGHT", 1, 256, 0);
  ASSERT_EQUALS(0, ec);
}

void test_decb_create() {
  decb_path_id p;
  error_code ec;

  // test create of a non-existing illegal file on a non-existing disk image
  ec = _decb_create(&p, "test_disk_doesnt_exist.dsk,file_doesnt_exist",
                    FAM_READ, 0, 1);
  ASSERT_EQUALS(EOS_BPNAM, ec);

  // test create of a non-existing legal file on a non-existing disk image
  ec = _decb_create(&p, "test_disk_doesnt_exist.dsk,FILE.TXT", FAM_READ, 0, 1);
  ASSERT_EQUALS(EOS_BPNAM, ec);

  // test create of a non-existing file on an existing disk image
  ec = _decb_create(&p, "test.dsk,test.txt:0", FAM_READ | FAM_WRITE, 0, 1);
  ASSERT_EQUALS(0, ec);
  ec = _decb_close(p);
  ASSERT_EQUALS(0, ec);

  // test create of an existing file on an existing disk image
  ec = _decb_create(&p, "test.dsk,test.txt", FAM_READ | FAM_WRITE, 0, 1);
  ASSERT_EQUALS(0, ec);
  ec = _decb_close(p);
  ASSERT_EQUALS(0, ec);

  // test create of an existing file on an existing disk image with FAM_NOCREATE
  ec = _decb_create(&p, "test.dsk,test.txt",
                    FAM_READ | FAM_WRITE | FAM_NOCREATE, 0, 1);
  ASSERT_EQUALS(EOS_FAE, ec);

  // test create of an extra long (illegal) filename on an existing disk image
  ec = _decb_create(&p,
                    "test.dsk,file_doesnt_exist_and_is_much_longer_than_decb_"
                    "limit_of_8_characters",
                    FAM_READ, 0, 1);
  ASSERT_EQUALS(EOS_BPNAM, ec);

  // test create of an extra long (illegal) filename with a subfolder on an
  // existing disk image the root file is a directory when it isn't
  ec = _decb_create(&p,
                    "test.dsk,file_doesnt_exist_and_is_much_longer_than_rbf_"
                    "limit_of_8_characters/"
                    "and_this_is_an_even_longer_name_than_the_8_character_"
                    "limit_in9_rbf_because_it_has_more_characters",
                    FAM_READ, 0, 1);
  ASSERT_EQUALS(EOS_BPNAM, ec);

  // Test filling up a disk. File 0 to 67 should succeed.
  // File 68 should fail
  ec = _decb_dskini("filldisk.dsk", 35, "FILL", 1, 256, 0);
  ASSERT_EQUALS(0, ec);

  for (int i = 0; i < 69; i++) {
    char path[256];

    snprintf(path, 255, "filldisk.dsk,TEST%d.BIN", i);

    ec = _decb_create(&p, path, FAM_READ | FAM_WRITE, 0, 1);
    if (i == 68) {
      ASSERT_EQUALS(EOS_DF, ec);
    } else {
      ASSERT_EQUALS(0, ec);
    }

    if (i != 68) {
      ec = _decb_close(p);
      ASSERT_EQUALS(0, ec);
    }
  }
}

void test_decb_read() {
  decb_path_id p;
  error_code ec;

  // test create of a non-existing file on an existing disk image
  ec = _decb_create(&p, "test.dsk,test2.txt", FAM_READ | FAM_WRITE, 0, 1);
  ASSERT_EQUALS(0, ec);

  // test read of an empty file
  char buff[32];
  u_int size = 32;
  ec = _decb_read(p, buff, &size);
  ASSERT_EQUALS(EOS_EOF, ec);

  // test close of a validly opened file
  ec = _decb_close(p);
  ASSERT_EQUALS(0, ec);
}

void test_decb_write() {
  decb_path_id p;
  error_code ec;

  // test create of a non-exsting file on an existing disk image
  ec = _decb_create(&p, "test.dsk,test3.txt", FAM_READ | FAM_WRITE, 0, 1);
  ASSERT_EQUALS(0, ec);

  // test write when file only open for read
  char *buff = "this is a string\nand this is another string\n";
  u_int size = strlen(buff);
  ec = _decb_write(p, buff, &size);
  ASSERT_EQUALS(0, ec);

  // test close of file
  ec = _decb_close(p);
  ASSERT_EQUALS(0, ec);

  // test create of a non-existing file on an existing disk image
  ec = _decb_create(&p, "test.dsk,test4.txt", FAM_READ | FAM_WRITE, 0, 1);
  ASSERT_EQUALS(0, ec);

  // test write when file open for read and write
  buff = "this is a string";
  size = strlen(buff);
  ec = _decb_write(p, buff, &size);
  ASSERT_EQUALS(0, ec);

  // test close of a validly opened file
  ec = _decb_close(p);
  ASSERT_EQUALS(0, ec);

  // Test filling up a disk with one file.
  ec = _decb_dskini("filldisk.dsk", 35, "FILL", 1, 256, 0);
  ASSERT_EQUALS(0, ec);

  ec = _decb_create(&p, "filldisk.dsk,test.bin", FAM_READ | FAM_WRITE, 0, 1);
  ASSERT_EQUALS(0, ec);

  size_t buffer_alloc = 68 * 2304;
  unsigned int buffer_size = buffer_alloc;
  unsigned char *buffer = calloc(1, buffer_size);
  ASSERT_EQUALS(1, buffer != NULL);

  for (int i = 0; i < buffer_alloc; i++) {
    buffer[i] = i;
  }

  ec = _decb_write(p, buffer, &buffer_size);
  ASSERT_EQUALS(0, ec);
  ASSERT_EQUALS(buffer_alloc, buffer_size);

  ec = _decb_close(p);
  ASSERT_EQUALS(0, ec);

  // Try adding one more file to a full disk. Should fail.
  ec = _decb_create(&p, "filldisk.dsk,test1.bin", FAM_READ | FAM_WRITE, 0, 1);
  ASSERT_EQUALS(EOS_DF, ec);

  free(buffer);

  // Nearly fill a test disk

  ec = _decb_dskini("filldisk.dsk", 35, "FILL", 1, 256, 0);
  ASSERT_EQUALS(0, ec);

  buffer_alloc = 67 * 2304;
  buffer_size = buffer_alloc;
  buffer = calloc(1, buffer_size);
  ASSERT_EQUALS(1, buffer != NULL);

  for (int i = 0; i < buffer_alloc; i++) {
    buffer[i] = i;
  }

  ec = _decb_create(&p, "filldisk.dsk,test.bin", FAM_READ | FAM_WRITE, 0, 1);
  ASSERT_EQUALS(0, ec);

  ec = _decb_write(p, buffer, &buffer_size);
  ASSERT_EQUALS(0, ec);
  ASSERT_EQUALS(buffer_alloc, buffer_size);

  ec = _decb_close(p);
  ASSERT_EQUALS(0, ec);

  // Try adding one more file to a nearly full disk. Should fail, file too big.
  ec = _decb_create(&p, "filldisk.dsk,test1.bin", FAM_READ | FAM_WRITE, 0, 1);
  ASSERT_EQUALS(0, ec);

  buffer_alloc = 2304 + 1;
  buffer_size = buffer_alloc;
  ec = _decb_write(p, buffer, &buffer_size);
  ASSERT_EQUALS(EOS_DF, ec);
  ASSERT_EQUALS(0, buffer_size);

  ec = _decb_close(p);
  ASSERT_EQUALS(0, ec);

  // Now delete the dangling file
  ec = _decb_kill("filldisk.dsk,test1.bin");
  ASSERT_EQUALS(0, ec);

  free(buffer);
}

void test_decb_open_and_read() {
  decb_path_id p;
  error_code ec;

  // test open of a non-existing disk image
  ec = _decb_open(&p, "test_disk_doesnt_exist.dsk,file_doesnt_exist", FAM_READ);
  ASSERT_EQUALS(EOS_BPNAM, ec);

  // test open of an existing file on an existing disk image
  ec = _decb_open(&p, "test.dsk,test4.txt", FAM_READ);
  ASSERT_EQUALS(0, ec);

  // test read of an existing file on an existing disk image
  char buf[256];
  u_int size = 128;

  ec = _decb_read(p, buf, &size);
  ASSERT_EQUALS(0, ec);
  ASSERT_EQUALS(16, size);

  // test read after all data has been read
  ec = _decb_read(p, buf, &size);
  ASSERT_EQUALS(EOS_EOF, ec);

  // test close of a validly opened file
  ec = _decb_close(p);
  ASSERT_EQUALS(0, ec);
}

void test_decb_delete() {
  error_code ec;

  // test deletion of a non-existing disk image
  ec = _decb_kill("test_disk_doesnt_exist.dsk,file_doesnt_exist");
  ASSERT_EQUALS(EOS_BPNAM, ec);

  // test deletion of a non-exsting file on an existing disk image
  ec = _decb_kill("test.dsk,test4.txt");
  ASSERT_EQUALS(0, ec);
}

void test_decb_rename() {
  error_code ec;

  // test rename of a non-existing file in an existing disk image
  ec = _decb_rename("test.dsk,file_doesnt_exist", "another_file_doesnt_exist");
  ASSERT_EQUALS(EOS_BPNAM, ec);

  // test rename of an existing file in an existing disk image with an illegal
  // name
  ec = _decb_rename("test.dsk,test3.txt", "test_renamed.txt");
  ASSERT_EQUALS(EOS_BPNAM, ec);

  // test rename of an existing file in an existing disk image with a legal name
  ec = _decb_rename("test.dsk,test3.txt", "testren.txt");
  ASSERT_EQUALS(0, ec);
}

char *basic_prog = "10 PRINT\"Hello World\"\n15 X=INT(234.54-23)\n20 GOTO 10\n";

void test_decb_token() {
  error_code ec;

  u_int source_length = strlen(basic_prog);
  unsigned char *source;
  unsigned char *entokenize_buffer;
  u_int entokenize_size;
  int path_type = 0;

  char *detokenize_buffer;
  u_int detokenize_size;

  source = malloc(source_length + 1);
  strcpy((char *)source, basic_prog);

  ec = _decb_entoken(source, source_length, &entokenize_buffer,
                     &entokenize_size, path_type);
  ASSERT_EQUALS(0, ec);

  ec = _decb_detoken(entokenize_buffer, entokenize_size, &detokenize_buffer,
                     &detokenize_size);
  ASSERT_EQUALS(0, ec);

  ASSERT_EQUALS(source_length, detokenize_size);

  detokenize_buffer[detokenize_size] = '\0';

  ASSERT_STRING_EQUALS(basic_prog, detokenize_buffer);

  free(source);
  free(entokenize_buffer);
  free(detokenize_buffer);
}

void test_decb_interleave() {
  error_code ec;

  /* Test format with interleave 3 (standard) */
  ec = _decb_dskini("test_ilv3.dsk", 35, "ILV3", 3, 256, 0);
  ASSERT_EQUALS(0, ec);

  /* Test format with interleave 1 */
  ec = _decb_dskini("test_ilv1.dsk", 35, "ILV1", 1, 256, 0);
  ASSERT_EQUALS(0, ec);

  remove("test_ilv3.dsk");
  remove("test_ilv1.dsk");
}

void test_decb_count_segments() {
  /* Test 1: Single segment followed by postamble */
  unsigned char single_seg[] = {
    0x00,             /* PREAMBLE */
    0x00, 0x02,       /* length = 2 */
    0x10, 0x00,       /* address = 0x1000 */
    0x12, 0x34,       /* data */
    0xFF,             /* POSTAMBLE */
    0x00, 0x00,       /* postamble zeros */
    0x10, 0x00        /* exec address */
  };
  ASSERT_EQUALS(1, _decb_count_segements(single_seg, sizeof(single_seg)));

  /* Test 2: Multiple segments followed by postamble */
  unsigned char multi_seg[] = {
    0x00,             /* PREAMBLE */
    0x00, 0x02,       /* length = 2 */
    0x10, 0x00,       /* address = 0x1000 */
    0x12, 0x34,       /* data */
    0x00,             /* PREAMBLE */
    0x00, 0x03,       /* length = 3 */
    0x20, 0x00,       /* address = 0x2000 */
    0xAA, 0xBB, 0xCC, /* data */
    0xFF,             /* POSTAMBLE */
    0x00, 0x00,       /* postamble zeros */
    0x10, 0x00        /* exec address */
  };
  ASSERT_EQUALS(2, _decb_count_segements(multi_seg, sizeof(multi_seg)));

  /* Test 3: Multiple segments with trailing junk after postamble */
  unsigned char junk_trailing[] = {
    0x00,
    0x00, 0x01,
    0x30, 0x00,
    0x42,
    0xFF,
    0x00, 0x00,
    0x30, 0x00,
    0x00, 0x01, 0x02  /* junk after postamble should not be parsed as extra segment */
  };
  ASSERT_EQUALS(1, _decb_count_segements(junk_trailing, sizeof(junk_trailing)));
}

void test_decb_binconcat() {
  error_code ec;
  unsigned char *out_buf = NULL;
  u_int out_size = 0;

  /* Input segment ending at address 0xFFFF (length 2 at 0xFFFE) */
  unsigned char in_buf[] = {
    0x00,             /* PREAMBLE */
    0x00, 0x02,       /* length = 2 */
    0xFF, 0xFE,       /* address = 0xFFFE */
    0xAA, 0xBB,       /* 0xAA at 0xFFFE, 0xBB at 0xFFFF */
    0xFF,             /* POSTAMBLE */
    0x00, 0x00,       /* zeros */
    0xFF, 0xFE        /* exec address */
  };

  ec = _decb_binconcat(in_buf, sizeof(in_buf), &out_buf, &out_size);
  ASSERT_EQUALS(0, ec);
  ASSERT("out_buf != NULL", out_buf != NULL);
  /* Output should be:
   * 0x00 (preamble), 0x00, 0x02 (length=2), 0xFF, 0xFE (address), 0xAA, 0xBB (data),
   * 0xFF (postamble), 0x00, 0x00, 0xFF, 0xFE (exec) -> total 12 bytes
   */
  ASSERT_EQUALS(12, out_size);
  ASSERT_EQUALS(0x00, out_buf[0]);
  ASSERT_EQUALS(0x00, out_buf[1]);
  ASSERT_EQUALS(0x02, out_buf[2]);
  ASSERT_EQUALS(0xFF, out_buf[3]);
  ASSERT_EQUALS(0xFE, out_buf[4]);
  ASSERT_EQUALS(0xAA, out_buf[5]);
  ASSERT_EQUALS(0xBB, out_buf[6]);
  ASSERT_EQUALS(0xFF, out_buf[7]);
  ASSERT_EQUALS(0x00, out_buf[8]);
  ASSERT_EQUALS(0x00, out_buf[9]);
  ASSERT_EQUALS(0xFF, out_buf[10]);
  ASSERT_EQUALS(0xFE, out_buf[11]);

  free(out_buf);
}

int main() {
  remove("test.dsk");
  remove("filldisk.dsk");
  remove("test_ilv3.dsk");
  remove("test_ilv1.dsk");

  RUN(test_decb_dskini);
  RUN(test_decb_create);
  RUN(test_decb_read);
  RUN(test_decb_write);
  RUN(test_decb_open_and_read);
  RUN(test_decb_delete);
  RUN(test_decb_rename);
  RUN(test_decb_token);
  RUN(test_decb_interleave);
  RUN(test_decb_count_segments);
  RUN(test_decb_binconcat);

  remove("test.dsk");
  remove("filldisk.dsk");
  remove("test_ilv3.dsk");
  remove("test_ilv1.dsk");

  return TEST_REPORT();
}
