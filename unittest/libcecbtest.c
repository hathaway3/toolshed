/*
 * ALWAYS BE TESTING!!!
 *
 * ALWAYS BE WRITING MORE TESTS!!!
 *
 * THIS WORK IS **NEVER** DONE!!!
 */

#include "tinytest.h"
#include <toolshed.h>

#define TEST_STRING "Line1\nLine2\n"
char *test_string = TEST_STRING;
unsigned int test_string_size = sizeof TEST_STRING;

void test_cecb_bulkerase()
{
	error_code ec;

	// test creation of a non-existent wave file
	ec = _cecb_bulkerase("test.wav", 44100, 8, 1.0);
	ASSERT_EQUALS(ec, 0);

	// test creation of a non-existent cas file
	ec = _cecb_bulkerase("test.cas", 0, 0, 0);
	ASSERT_EQUALS(ec, 0);
}

void test_cecb_create()
{
	cecb_path_id p_wav;
	cecb_path_id p_cas;
	error_code ec;
	unsigned int size;

	// Try to make a file with a name too long
	ec = _cecb_create(&p_wav, "test.wav,TOOLONGFILENAME",
			  FAM_READ | FAM_WRITE, 1, 255, 0, 0x55, 0x65);
	ASSERT_EQUALS(ec, EOS_BPNAM);

	// Try to make a file with a name too long
	ec = _cecb_create(&p_cas, "test.cas,TOOLONGFILENAME",
			  FAM_READ | FAM_WRITE, 1, 255, 0, 0x55, 0x65);
	ASSERT_EQUALS(ec, EOS_BPNAM);

	// Open file written in previous written
	ec = _cecb_create(&p_wav, "test.wav,FILE", FAM_READ | FAM_WRITE, 1,
			  255, 0, 0x55, 0x65);
	ASSERT_EQUALS(ec, 0);

	// Open file written in previous written
	ec = _cecb_create(&p_cas, "test.cas,FILE", FAM_READ | FAM_WRITE, 1,
			  255, 0, 0x55, 0x65);
	ASSERT_EQUALS(ec, 0);

	// Write data to file
	size = test_string_size;
	ec = _cecb_write(p_wav, test_string, &size);
	ASSERT_EQUALS(ec, 0);
	ASSERT_EQUALS(size, test_string_size);

	// Write data to file
	size = test_string_size;
	ec = _cecb_write(p_cas, test_string, &size);
	ASSERT_EQUALS(ec, 0);
	ASSERT_EQUALS(size, test_string_size);

	ec = _cecb_close(p_wav);
	ASSERT_EQUALS(ec, 0);

	ec = _cecb_close(p_cas);
	ASSERT_EQUALS(ec, 0);
}

void test_cecb_read()
{
	cecb_path_id p_wav;
	cecb_path_id p_cas;
	error_code ec;
	char buffer[512];
	unsigned int size;

	// Opening in write mode is not supported
	ec = _cecb_open(&p_wav, "DoesNoteExit.wav,FILE",
			FAM_READ | FAM_WRITE);
	ASSERT_EQUALS(ec, EOS_IC);

	// Opening in write mode is not supported
	ec = _cecb_open(&p_wav, "DoesNoteExit.wav,FILE", FAM_READ);
	ASSERT_EQUALS(ec, EOS_BPNAM);

	// Opening existing file
	ec = _cecb_open(&p_cas, "DoesNoteExit.cas,FILE", FAM_READ);
	ASSERT_EQUALS(ec, EOS_BPNAM);

	// Opening existing file
	ec = _cecb_open(&p_wav, "test.wav,FILE", FAM_READ);
	ASSERT_EQUALS(ec, 0);

	// Opening existing file
	ec = _cecb_open(&p_cas, "test.cas,FILE", FAM_READ);
	ASSERT_EQUALS(ec, 0);

	// Read file into buffer
	size = 512;
	ec = _cecb_read(p_wav, buffer, &size);
	ASSERT_EQUALS(ec, 0);
	ASSERT_EQUALS(size, test_string_size);
	ASSERT_STRING_EQUALS(buffer, test_string);

	// Read file into buffer
	size = 512;
	buffer[0] = 0;
	ec = _cecb_read(p_cas, buffer, &size);
	ASSERT_EQUALS(ec, 0);
	ASSERT_EQUALS(size, test_string_size);
	ASSERT_STRING_EQUALS(buffer, test_string);

	ec = _cecb_close(p_wav);
	ASSERT_EQUALS(ec, 0);

	ec = _cecb_close(p_cas);
	ASSERT_EQUALS(ec, 0);
}

int main()
{
	RUN(test_cecb_bulkerase);
	RUN(test_cecb_create);
	RUN(test_cecb_read);

	return TEST_REPORT();
}
