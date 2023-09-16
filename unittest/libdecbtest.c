/*
 * ALWAYS BE TESTING!!!
 *
 * ALWAYS BE WRITING MORE TESTS!!!
 *
 * THIS WORK IS **NEVER** DONE!!!
 */

#include "tinytest.h"
#include <toolshed.h>

void test_decb_dskini()
{
	error_code ec;

	// test format of a non-existent disk image
	ec = _decb_dskini("test.dsk", 35, "TEST", 1, 256, 0);
	ASSERT_EQUALS(ec, 0);

	// test format of an existing disk image with a disk name that is way too long
	ec = _decb_dskini("test.dsk", 35,
			  "TESTDISKNAMEISWAYTOOLONGFORNORMALOPERATION", 1,
			  256, 0);
	ASSERT_EQUALS(ec, EOS_BPNAM);

	// test format of an existing disk image with a disk name that is barely too long
	ec = _decb_dskini("test.dsk", 35, "TADTTOOLONG", 1, 256, 0);
	ASSERT_EQUALS(ec, EOS_BPNAM);

	// test format of an existing disk image with a disk name that is the max length
	ec = _decb_dskini("test.dsk", 35, "JUSTRIGHT", 1, 256, 0);
	ASSERT_EQUALS(ec, 0);
}

void test_decb_create()
{
	decb_path_id p;
	error_code ec;

	// test create of a non-existing illegal file on a non-existing disk image
	ec = _decb_create(&p, "test_disk_doesnt_exist.dsk,file_doesnt_exist",
			  FAM_READ, 0, 1);
	ASSERT_EQUALS(ec, EOS_BPNAM);

	// test create of a non-existing legal file on a non-existing disk image
	ec = _decb_create(&p, "test_disk_doesnt_exist.dsk,FILE.TXT", FAM_READ,
			  0, 1);
	ASSERT_EQUALS(ec, EOS_BPNAM);

	// test create of a non-existing file on an existing disk image
	ec = _decb_create(&p, "test.dsk,test.txt:0", FAM_READ | FAM_WRITE, 0,
			  1);
	ASSERT_EQUALS(ec, 0);
	ec = _decb_close(p);
	ASSERT_EQUALS(ec, 0);

	// test create of an existing file on an existing disk image
	ec = _decb_create(&p, "test.dsk,test.txt", FAM_READ | FAM_WRITE, 0,
			  1);
	ASSERT_EQUALS(ec, 0);
	ec = _decb_close(p);
	ASSERT_EQUALS(ec, 0);

	// test create of an existing file on an existing disk image with FAM_NOCREATE
	ec = _decb_create(&p, "test.dsk,test.txt",
			  FAM_READ | FAM_WRITE | FAM_NOCREATE, 0, 1);
	ASSERT_EQUALS(ec, EOS_FAE);

	// test create of an extra long (illegal) filename on an existing disk image
	ec = _decb_create(&p,
			  "test.dsk,file_doesnt_exist_and_is_much_longer_than_decb_limit_of_8_characters",
			  FAM_READ, 0, 1);
	ASSERT_EQUALS(ec, EOS_BPNAM);

	// test create of an extra long (illegal) filename with a subfolder on an existing disk image
	// the root file is a directory when it isn't
	ec = _decb_create(&p,
			  "test.dsk,file_doesnt_exist_and_is_much_longer_than_rbf_limit_of_8_characters/and_this_is_an_even_longer_name_than_the_8_character_limit_in9_rbf_because_it_has_more_characters",
			  FAM_READ, 0, 1);
	ASSERT_EQUALS(ec, EOS_BPNAM);
	
	// Test filling up a disk. File 0 to 67 should succeed.
	// File 68 should fail
	ec = _decb_dskini("filldisk.dsk", 35, "FILL", 1, 256, 0);
	ASSERT_EQUALS(ec, 0);

	for (int i=0; i<69; i++)
	{
		char path[256];
		
		snprintf(path, 255, "filldisk.dsk,TEST%d.BIN", i);

		ec = _decb_create(&p, path, FAM_READ | FAM_WRITE, 0,
				  1);
		if (i==68)
		{
			ASSERT_EQUALS(ec, EOS_DF);
		}
		else
		{
			ASSERT_EQUALS(ec, 0);
		}
		
		if (i!=68)
		{
			ec = _decb_close(p);
			ASSERT_EQUALS(ec, 0);
		}
	}
}

void test_decb_read()
{
	decb_path_id p;
	error_code ec;

	// test create of a non-existing file on an existing disk image
	ec = _decb_create(&p, "test.dsk,test2.txt", FAM_READ | FAM_WRITE, 0,
			  1);
	ASSERT_EQUALS(ec, 0);

	// test read of an empty file
	char buff[32];
	u_int size = 32;
	ec = _decb_read(p, buff, &size);
	ASSERT_EQUALS(ec, EOS_EOF);

	// test close of a validly opened file
	ec = _decb_close(p);
	ASSERT_EQUALS(ec, 0);
}

void test_decb_write()
{
	decb_path_id p;
	error_code ec;

	// test create of a non-exsting file on an existing disk image
	ec = _decb_create(&p, "test.dsk,test3.txt", FAM_READ | FAM_WRITE, 0,
			  1);
	ASSERT_EQUALS(ec, 0);

	// test write when file only open for read
	char *buff = "this is a string\nand this is another string\n";
	u_int size = strlen(buff);
	ec = _decb_write(p, buff, &size);
	ASSERT_EQUALS(ec, 0);

	// test close of file
	ec = _decb_close(p);
	ASSERT_EQUALS(ec, 0);

	// test create of a non-existing file on an existing disk image
	ec = _decb_create(&p, "test.dsk,test4.txt", FAM_READ | FAM_WRITE, 0,
			  1);
	ASSERT_EQUALS(ec, 0);

	// test write when file open for read and write
	buff = "this is a string";
	size = strlen(buff);
	ec = _decb_write(p, buff, &size);
	ASSERT_EQUALS(ec, 0);

	// test close of a validly opened file
	ec = _decb_close(p);
	ASSERT_EQUALS(ec, 0);
	
	// Test filling up a disk with one file.
	ec = _decb_dskini("filldisk.dsk", 35, "FILL", 1, 256, 0);
	ASSERT_EQUALS(ec, 0);

	ec = _decb_create(&p, "filldisk.dsk,test.bin", FAM_READ | FAM_WRITE, 0,
			  1);
	ASSERT_EQUALS(ec, 0);
	
	size_t buffer_alloc = 68*2304;
	unsigned int buffer_size = buffer_alloc;
	unsigned char *buffer = calloc(1, buffer_size);
	ASSERT_NEQUALS(buffer, 0);
	
	for( int i=0; i<buffer_alloc; i++)
	{
		buffer[i] = i;
	}
	
	ec = _decb_write(p, buffer, &buffer_size);
	ASSERT_EQUALS(ec, 0);
	ASSERT_EQUALS(buffer_size, buffer_alloc);

	ec = _decb_close(p);
	ASSERT_EQUALS(ec, 0);

	// Try adding one more file to a full disk. Should fail.
	ec = _decb_create(&p, "filldisk.dsk,test1.bin", FAM_READ | FAM_WRITE, 0,
			  1);
	ASSERT_EQUALS(ec, EOS_DF);
	
	free(buffer);
	
	// Nearly fill a test disk
	
	ec = _decb_dskini("filldisk.dsk", 35, "FILL", 1, 256, 0);
	ASSERT_EQUALS(ec, 0);

	buffer_alloc = 67*2304;
	buffer_size = buffer_alloc;
	buffer = calloc(1, buffer_size);
	ASSERT_NEQUALS(buffer, 0);
	
	for( int i=0; i<buffer_alloc; i++)
	{
		buffer[i] = i;
	}
	
	ec = _decb_create(&p, "filldisk.dsk,test.bin", FAM_READ | FAM_WRITE, 0,
			  1);
	ASSERT_EQUALS(ec, 0);

	ec = _decb_write(p, buffer, &buffer_size);
	ASSERT_EQUALS(ec, 0);
	ASSERT_EQUALS(buffer_size, buffer_alloc);

	ec = _decb_close(p);
	ASSERT_EQUALS(ec, 0);

	// Try adding one more file to a nearly full disk. Should fail, file too big.
	ec = _decb_create(&p, "filldisk.dsk,test1.bin", FAM_READ | FAM_WRITE, 0,
			  1);
	ASSERT_EQUALS(ec, 0);
	
	buffer_alloc = 2304 + 1;
	buffer_size = buffer_alloc;
	ec = _decb_write(p, buffer, &buffer_size);
	ASSERT_EQUALS(EOS_DF, ec);
	ASSERT_EQUALS(0, buffer_size);

	ec = _decb_close(p);
	ASSERT_EQUALS(ec, 0);
	
	// Now delete the dangling file
	ec = _decb_kill("filldisk.dsk,test1.bin");
	ASSERT_EQUALS(ec, 0);

	free(buffer);
}

void test_decb_open_and_read()
{
	decb_path_id p;
	error_code ec;

	// test open of a non-existing disk image
	ec = _decb_open(&p, "test_disk_doesnt_exist.dsk,file_doesnt_exist",
			FAM_READ);
	ASSERT_EQUALS(ec, EOS_BPNAM);

	// test open of an existing file on an existing disk image
	ec = _decb_open(&p, "test.dsk,test4.txt", FAM_READ);
	ASSERT_EQUALS(ec, 0);

	// test read of an existing file on an existing disk image
	char buf[256];
	u_int size = 128;

	ec = _decb_read(p, buf, &size);
	ASSERT_EQUALS(ec, 0);
	ASSERT_EQUALS(size, 16);

	// test read after all data has been read
	ec = _decb_read(p, buf, &size);
	ASSERT_EQUALS(ec, EOS_EOF);

	// test close of a validly opened file
	ec = _decb_close(p);
	ASSERT_EQUALS(ec, 0);
}

void test_decb_delete()
{
	error_code ec;

	// test deletion of a non-existing disk image
	ec = _decb_kill("test_disk_doesnt_exist.dsk,file_doesnt_exist");
	ASSERT_EQUALS(ec, EOS_BPNAM);

	// test deletion of a non-exsting file on an existing disk image
	ec = _decb_kill("test.dsk,test4.txt");
	ASSERT_EQUALS(ec, 0);
}

void test_decb_rename()
{
	error_code ec;

	// test rename of a non-existing file in an existing disk image
	ec = _decb_rename("test.dsk,file_doesnt_exist",
			  "another_file_doesnt_exist");
	ASSERT_EQUALS(ec, EOS_BPNAM);

	// test rename of an existing file in an existing disk image with an illegal name
	ec = _decb_rename("test.dsk,test3.txt", "test_renamed.txt");
	ASSERT_EQUALS(ec, EOS_BPNAM);

	// test rename of an existing file in an existing disk image with a legal name
	ec = _decb_rename("test.dsk,test3.txt", "testren.txt");
	ASSERT_EQUALS(ec, 0);
}

int main()
{
	remove("test.dsk");
	remove("filldisk.dsk");
	RUN(test_decb_dskini);
	RUN(test_decb_create);
	RUN(test_decb_read);
	RUN(test_decb_write);
	RUN(test_decb_open_and_read);
	RUN(test_decb_delete);
	RUN(test_decb_rename);

	return TEST_REPORT();
}
