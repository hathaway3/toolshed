/*
 * ALWAYS BE TESTING!!!
 *
 * ALWAYS BE WRITING MORE TESTS!!!
 *
 * THIS WORK IS **NEVER** DONE!!!
 */

#include "tinytest.h"
#include <toolshed.h>

void test_os9_format()
{
	error_code ec;

	// test format of a non-existent disk image
	unsigned int totalSectors, totalBytes;
	ec = _os9_format("test.dsk", 0, 35, 18, 1, 256, 1, "Test Disk", 8, 8, 1, 1, 0, 0, &totalSectors, &totalBytes);
	ASSERT_EQUALS(ec, 0);

	// test format of a non-existent disk image with a disk name that is way too long
	ec = _os9_format("test.dsk", 0, 35, 18, 1, 256, 1, "Test Disk with filename that is way too long for the field", 8, 8, 1, 1, 0, 0, &totalSectors, &totalBytes);
	ASSERT_EQUALS(ec, 0);

	// TODO: test format with oddball parameters to make sure it can survive
}

void test_os9_create()
{
	os9_path_id p;
	error_code ec;

	// test create of a non-existing file on a non-existing disk image
	ec = _os9_create(&p, "test_disk_doesnt_exist.dsk,file_doesnt_exist", FAM_READ, FAP_READ);
	ASSERT_EQUALS(ec, EOS_PNNF);

	// test create of a non-existing file on an existing disk image
	ec = _os9_create(&p, "test.dsk,test.txt", FAM_READ, FAP_READ);
	ASSERT_EQUALS(ec, 0);
	ec = _os9_close(p);
	ASSERT_EQUALS(ec, 0);

	// test create of an existing file on an existing disk image
	ec = _os9_create(&p, "test.dsk,test.txt", FAM_READ, FAP_READ);
	ASSERT_EQUALS(ec, 0);
	ec = _os9_close(p);
	ASSERT_EQUALS(ec, 0);

	// test create of an existing file on an existing disk image with FAM_NOCREATE
	ec = _os9_create(&p, "test.dsk,test.txt", FAM_READ | FAM_NOCREATE, FAP_READ);
	ASSERT_EQUALS(ec, EOS_FAE);

	// test create of an extra long (illegal) filename on an existing disk image
	ec = _os9_create(&p, "test.dsk,file_doesnt_exist_and_is_much_longer_than_os9_limit_of_29_characters", FAM_READ, FAP_READ);
	ASSERT_EQUALS(ec, EOS_BPNAM);

	// test create of an extra long (illegal) filename with a subfolder on an existing disk image
	// the root file is a directory when it isn't
	ec = _os9_create(&p, "test.dsk,file_doesnt_exist_and_is_much_longer_than_rbf_limit_of_29_characters/and_this_is_an_even_longer_name_than_the_29_character_limit_in9_rbf_because_it_has_more_characters", FAM_READ, FAP_READ);
	ASSERT_EQUALS(ec, EOS_PNNF);
}

void test_os9_read()
{
	os9_path_id p;
	error_code ec;

	// test create of a non-existing file on an existing disk image
	ec = _os9_create(&p, "test.dsk,test2.txt", FAM_READ, FAP_READ);
	ASSERT_EQUALS(ec, 0);

	// test read of an empty file
	char buff[32];
	u_int size = 32;
	ec = _os9_read(p, buff, &size);
	ASSERT_EQUALS(ec, EOS_EOF);

	// test close of a validly opened file
	ec = _os9_close(p);
	ASSERT_EQUALS(ec, 0);
}

void test_os9_write()
{
	os9_path_id p;
	error_code ec;

	// test create of a non-exsting file on an existing disk image
	ec = _os9_create(&p, "test.dsk,test3.txt", FAM_READ, FAP_READ);
	ASSERT_EQUALS(ec, 0);

	// test write when file only open for read
	char *buff = "this is a string\nand this is another string\n";
	u_int size = strlen(buff);
	ec = _os9_write(p, buff, &size);
	ASSERT_EQUALS(ec, EOS_BMODE);

	// test close of file
	ec = _os9_close(p);
	ASSERT_EQUALS(ec, 0);

	// test create of a non-existing file on an existing disk image
	ec = _os9_create(&p, "test.dsk,test4.txt", FAM_READ | FAM_WRITE, FAP_READ | FAP_WRITE);
	ASSERT_EQUALS(ec, 0);

	// test write when file open for read and write
	buff = "this is a string";
	size = strlen(buff);
	ec = _os9_write(p, buff, &size);
	ASSERT_EQUALS(ec, 0);

	// test close of a validly opened file
	ec = _os9_close(p);
	ASSERT_EQUALS(ec, 0);
}

void test_os9_open_and_read()
{
	os9_path_id p;
	error_code ec;

	// test open of a non-existing disk image
	ec = _os9_open(&p, "test_disk_doesnt_exist.dsk,file_doesnt_exist", FAM_READ);
	ASSERT_EQUALS(ec, EOS_PNNF);

	// test open of an existing file on an existing disk image
	ec = _os9_open(&p, "test.dsk,test4.txt", FAM_READ);
	ASSERT_EQUALS(ec, 0);

	// test read of an existing file on an existing disk image
	char buf[256];
	u_int size = 128;

	ec = _os9_read(p, buf, &size);
	ASSERT_EQUALS(ec, 0);
	ASSERT_EQUALS(size, 16);

	// test read after all data has been read
	ec = _os9_read(p, buf, &size);
	ASSERT_EQUALS(ec, EOS_EOF);

	// test close of a validly opened file
	ec = _os9_close(p);
	ASSERT_EQUALS(ec, 0);
}

void test_os9_delete()
{
	error_code ec;

	// test deletion of a non-existing disk image
	ec = _os9_delete("test_disk_doesnt_exist.dsk,file_doesnt_exist");
	ASSERT_EQUALS(ec, EOS_PNNF);

	// test deletion of a non-exsting file on an existing disk image
	ec = _os9_delete("test.dsk,test4.txt");
	ASSERT_EQUALS(ec, 0);
}

void test_os9_makdir()
{
	error_code ec;

	// test create of a non-existing directory in an existing disk image
	ec = _os9_makdir("test.dsk,dir_doesnt_exist");
	ASSERT_EQUALS(ec, 0);

	// test create of an existing directory in an existing disk image
	ec = _os9_makdir("test.dsk,dir_doesnt_exist");
	ASSERT_EQUALS(ec, EOS_FAE);

	// test create of a non-existing subdirectory in a existing directory in an existing disk image
	ec = _os9_makdir("test.dsk,dir_doesnt_exist/sub_dir_doesnt_exist");
	ASSERT_EQUALS(ec, 0);
}

void test_os9_delete_directory()
{
	error_code ec;

	// test deletion of an existing directory in an existing disk image
	ec = _os9_delete_directory("test.dsk,dir_doesnt_exist");
	ASSERT_EQUALS(ec, 0);
}

void test_os9_rename()
{
	error_code ec;

	// test rename of a non-existing file in an existing disk image
	ec = _os9_rename("test.dsk,file_doesnt_exist", "another_file_doesnt_exist");
	ASSERT_EQUALS(ec, EOS_PNNF);

	// test rename of an existing file in an existing disk image
	ec = _os9_rename("test.dsk,test3.txt", "test_renamed.txt");
	ASSERT_EQUALS(ec, 0);
}

void test_os9_gs_calls()
{
	error_code ec;
	os9_path_id p;
	int perms = FAP_READ | FAP_WRITE;

	// test create of a non-existing file in an existing disk image
	ec = _os9_create(&p, "test.dsk,newfile.txt", FAM_READ | FAM_WRITE, perms);
	ASSERT_EQUALS(ec, 0);

	int gsperms;
	ec = _os9_gs_attr(p, &gsperms);
	ASSERT_EQUALS(ec, 0);
	ASSERT_EQUALS(gsperms, perms);

	// test close of a validly opened file
	ec = _os9_close(p);
	ASSERT_EQUALS(ec, 0);
}

void test_os9_ss_calls()
{
	// TODO: Write tests
}

int main()
{
	RUN(test_os9_format);
	RUN(test_os9_create);
	RUN(test_os9_read);
	RUN(test_os9_write);
	RUN(test_os9_open_and_read);
	RUN(test_os9_delete);
	RUN(test_os9_makdir);
	RUN(test_os9_delete_directory);
	RUN(test_os9_rename);
	RUN(test_os9_ss_calls);
	RUN(test_os9_gs_calls);

	return TEST_REPORT();
}
