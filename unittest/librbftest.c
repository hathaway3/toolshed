#include "tinytest.h"
#include <toolshed.h>

void test_os9_open()
{
	os9_path_id p;
	error_code ec;

	// test open of a non-existent disk image
	ec = _os9_open(&p, "test_disk_doesnt_exist.dsk,file_doesnt_exist", FAM_READ);
	ASSERT_EQUALS(ec, 216);

	// test open of an existing disk image with a non-existent file
	ec = _os9_open(&p, "test.dsk,file_doesnt_exist", FAM_READ);

	// test open and close of an existing disk image with an an existing file
	ec = _os9_open(&p, "test.dsk,test.txt", FAM_READ);
	ASSERT_EQUALS(ec, 0);
	ec = _os9_close(p);
	ASSERT_EQUALS(ec, 0);

	// test open and close of an existing disk image with an an existing file
	ec = _os9_open(&p, "test.dsk,test.txt", FAM_READ);
	ASSERT_EQUALS(ec, 0);
	ec = _os9_close(p);
	ASSERT_EQUALS(ec, 0);
}

void test_os9_create()
{
	os9_path_id p;
	error_code ec;

	// test create of a non-existent disk image
	ec = _os9_create(&p, "test_disk_doesnt_exist.dsk,file_doesnt_exist", FAM_READ, FAP_READ);
	ASSERT_EQUALS(ec, 216);

	// test create of an existing disk image with a non-existent file
	ec = _os9_create(&p, "test.dsk,file_doesnt_exist", FAM_READ, FAP_READ);
	ASSERT_EQUALS(ec, 0);
	ec = _os9_close(p);
	ASSERT_EQUALS(ec, 0);

	// test create of an existing disk image with an existing file
	ec = _os9_create(&p, "test.dsk,file_doesnt_exist", FAM_READ, FAP_READ);
	ASSERT_EQUALS(ec, 0);
	ec = _os9_close(p);
	ASSERT_EQUALS(ec, 0);

	// test create of an existing disk image with an existing file with FAM_NOCREATE
	ec = _os9_create(&p, "test.dsk,file_doesnt_exist", FAM_READ | FAM_NOCREATE, FAP_READ);
	ASSERT_EQUALS(ec, 218);
}

int main()
{
	RUN(test_os9_open);
	RUN(test_os9_create);
	return TEST_REPORT();
}
