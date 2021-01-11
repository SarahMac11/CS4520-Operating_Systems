#include <cstdlib>
#include <iostream>
#include <new>
#include <vector>
using std::vector;
using std::string;
#include <gtest/gtest.h>

extern "C" 
{
#include "FS.h"
}

unsigned int score;
unsigned int total;
class GradeEnvironment : public testing::Environment 
{
	public:
		virtual void SetUp() 
		{
			score = 0;

			total = 137;
		}

		virtual void TearDown() 
		{
			::testing::Test::RecordProperty("points_given", score);
			::testing::Test::RecordProperty("points_total", total);
			std::cout << "SCORE: " << score << '/' << total << std::endl;
		}
};

bool find_in_directory(const dyn_array_t *const record_arr, const char *fname) 
{
	if (record_arr && fname) 
	{
		for (size_t i = 0; i < dyn_array_size(record_arr); ++i) 
		{
			if (strncmp(((file_record_t *) dyn_array_at(record_arr, i))->name, fname, FS_FNAME_MAX) == 0) 
			{
				return true;
			}
		}
	}

	return false;
}


/*
   FS * fs_format(const char *const fname);
   1   Normal
   2   NULL
   3   Empty string
   FS *fs_mount(const char *const fname);
   1   Normal
   2   NULL
   3   Empty string
   int fs_unmount(FS *fs);
   1   Normal
   2   NULL
 */

TEST(a_tests, format_mount_unmount) 
{
	const char *test_fname = "a_tests.FS";
	FS *fs = NULL;
	// FORMAT 2
	ASSERT_EQ(fs_format(NULL), nullptr);
	// FORMAT 3
	// this really should just be caught by block_store
	ASSERT_EQ(fs_format(""), nullptr);
	// FORMAT 1
	fs = fs_format(test_fname);
	ASSERT_NE(fs, nullptr);
	// UNMOUNT 1
	ASSERT_EQ(fs_unmount(fs), 0);
	// UNMOUNT 2
	ASSERT_LT(fs_unmount(NULL), 0);
	// MOUNT 1
	fs = fs_mount(test_fname);
	ASSERT_NE(fs, nullptr);
	fs_unmount(fs);
	// MOUNT 2
	ASSERT_EQ(fs_mount(NULL), nullptr);
	// MOUNT 3
	ASSERT_EQ(fs_mount(""), nullptr);
}

/*
   int fs_create(FS *const fs, const char *const fname, const ftype_t ftype);
   1. Normal, file, in root
   2. Normal, directory, in root
   3. Normal, file, not in root
   4. Normal, directory, not in root
   5. Error, NULL fs
   6. Error, NULL fname
   7. Error, empty fname
   8. Error, bad type
   9. Error, path does not exist
   10. Error, Root clobber
   11. Error, already exists
   12. Error, file exists
   13. Error, part of path not directory
   14. Error, path terminal not directory
   15. Error, path string has no leading slash
   16. Error, path has trailing slash (no name for desired file)
   17. Error, bad path, path part too long
   18. Error, bad path, desired filename too long
   19. Error, directory full.
   20. Error, out of inodes.
   21. Error, out of data blocks & file is directory (requires functional write)
 */
TEST(b_tests, file_creation_one) 
{
	vector<const char *> filenames
	{
		"/file", "/folder", "/folder/with_file", "/folder/with_folder", "/DOESNOTEXIST", "/file/BAD_REQUEST",
			"/DOESNOTEXIST/with_file", "/folder/with_file/bad_req", "folder/missing_slash", "/folder/new_folder/",
			"/folder/withwaytoolongfilenamethattakesupmorespacethanitshould and yet was not enough so I had to add "
				"more/bad_req",
			"/folder/withfilethatiswayyyyytoolongwhydoyoumakefilesthataretoobigEXACT!", "/", "/mystery_file"
	};

	const char *test_fname = "b_tests_normal.FS";
	FS *fs = fs_format(test_fname);
	ASSERT_NE(fs, nullptr);

	// CREATE_FILE 1
	ASSERT_EQ(fs_create(fs, filenames[0], FS_REGULAR), 0);


	// CREATE_FILE 2
	ASSERT_EQ(fs_create(fs, filenames[1], FS_DIRECTORY), 0);


	// CREATE_FILE 3
	ASSERT_EQ(fs_create(fs, filenames[2], FS_REGULAR), 0);


	// CREATE_FILE 4
	ASSERT_EQ(fs_create(fs, filenames[3], FS_DIRECTORY), 0);


	// CREATE_FILE 5
	ASSERT_LT(fs_create(NULL, filenames[4], FS_REGULAR), 0);
	score += 5;

	// CREATE_FILE 6
	ASSERT_LT(fs_create(fs, NULL, FS_REGULAR), 0);


	// CREATE_FILE 7
	ASSERT_LT(fs_create(fs, "", FS_REGULAR), 0);


	// CREATE_FILE 8
	ASSERT_LT(fs_create(fs, filenames[13], (file_t) 44), 0);


	// CREATE_FILE 9
	ASSERT_LT(fs_create(fs, filenames[6], FS_REGULAR), 0);


	// CREATE_FILE 10
	ASSERT_LT(fs_create(fs, filenames[12], FS_DIRECTORY), 0);


	// CREATE_FILE 11
	ASSERT_LT(fs_create(fs, filenames[1], FS_DIRECTORY), 0);
	score += 5;

	ASSERT_LT(fs_create(fs, filenames[1], FS_REGULAR), 0);


	// CREATE_FILE 12
	ASSERT_LT(fs_create(fs, filenames[0], FS_REGULAR), 0);


	ASSERT_LT(fs_create(fs, filenames[0], FS_DIRECTORY), 0);


	// CREATE_FILE 13
	ASSERT_LT(fs_create(fs, filenames[5], FS_REGULAR), 0);


	// CREATE_FILE 14
	ASSERT_LT(fs_create(fs, filenames[7], FS_REGULAR), 0);


	// CREATE_FILE 15
	ASSERT_LT(fs_create(fs, filenames[8], FS_REGULAR), 0);

	// But if we don't support relative paths, is there a reason to force abolute notation?
	// It's really a semi-arbitrary restriction
	// I suppose relative paths are up to the implementation, since . and .. are just special folder entires
	// but that would mess with the directory content total, BUT extra parsing can work around that.
	// Hmmmm.
	// CREATE_FILE 16
	ASSERT_LT(fs_create(fs, filenames[9], FS_DIRECTORY), 0);


	// CREATE_FILE 17
	ASSERT_LT(fs_create(fs, filenames[10], FS_REGULAR), 0);


	// CREATE_FILE 18
	ASSERT_LT(fs_create(fs, filenames[11], FS_REGULAR), 0);


	// Closing this file now for inspection to make sure these tests didn't mess it up
	fs_unmount(fs);
	score += 5;
}



/*
   int fs_open(FS *fs, const char *path)
   1. Normal, file at root
   2. Normal, file in subdir
   3. Normal, multiple fd to the same file
   4. Error, NULL fs
   5. Error, NULL fname
   6. Error, empty fname ???
   7. Error, not a regular file
   8. Error, file does not exist
   9. Error, out of descriptors
   int fs_close(FS *fs, int fd);
   1. Normal, whatever
   2. Normal, attempt to use after closing, assert failure **
   3. Normal, multiple opens, close does not affect the others **
   4. Error, FS null
   5. Error, invalid fd, positive
   6. Error, invalid fd, positive, out of bounds
   7. Error, invaid fs, negative
 */

TEST(c_tests, open_close_file) 
{
	vector<const char *> filenames
	{
		"/file", "/folder", "/folder/with_file", "/folder/with_folder", "/DOESNOTEXIST", "/file/BAD_REQUEST",
			"/DOESNOTEXIST/with_file", "/folder/with_file/bad_req", "folder/missing_slash", "/folder/new_folder/",
			"/folder/withwaytoolongfilenamethattakesupmorespacethanitshould and yet was not enough so I had to add "
				"more/bad_req",
			"/folder/withfilethatiswayyyyytoolongwhydoyoumakefilesthataretoobigEXACT!", "/", "/mystery_file"
	};
	const char *test_fname = "c_tests.FS";
	ASSERT_EQ(system("cp b_tests_normal.FS c_tests.FS"), 0);
	FS *fs = fs_mount(test_fname);
	ASSERT_NE(fs, nullptr);
	int fd_array[256] = {-1};
	// OPEN_FILE 1
	fd_array[0] = fs_open(fs, filenames[0]);
	ASSERT_GE(fd_array[0], 0);


	// CLOSE_FILE 4
	ASSERT_LT(fs_close(NULL, fd_array[0]), 0);


	// CLOSE_FILE 1
	ASSERT_EQ(fs_close(fs, fd_array[0]), 0);


	// CLOSE_FILE 2 and 3 elsewhere
	// CLOSE_FILE 5
	ASSERT_LT(fs_close(fs, 70), 0);


	// CLOSE_FILE 6
	ASSERT_LT(fs_close(fs, 7583), 0);


	// CLOSE_FILE 7
	ASSERT_LT(fs_close(fs, -18), 0);
	score += 20;

	// OPEN_FILE 2
	fd_array[1] = fs_open(fs, filenames[2]);
	ASSERT_GE(fd_array[1], 0);
	ASSERT_EQ(fs_close(fs, fd_array[0]), 0);


	// OPEN_FILE 3
	fd_array[2] = fs_open(fs, filenames[0]);
	ASSERT_GE(fd_array[2], 0);
	fd_array[3] = fs_open(fs, filenames[0]);
	ASSERT_GE(fd_array[3], 0);
	fd_array[4] = fs_open(fs, filenames[0]);
	ASSERT_GE(fd_array[4], 0);
	ASSERT_EQ(fs_close(fs, fd_array[2]), 0);
	ASSERT_EQ(fs_close(fs, fd_array[3]), 0);
	ASSERT_EQ(fs_close(fs, fd_array[4]), 0);


	// OPEN_FILE 4
	fd_array[5] = fs_open(NULL, filenames[0]);
	ASSERT_LT(fd_array[5], 0);


	// OPEN_FILE 5
	fd_array[5] = fs_open(fs, NULL);
	ASSERT_LT(fd_array[5], 0);


	// OPEN_FILE 6
	// Uhh, bad filename? Not a slash?
	// It's wrong for a bunch of reasons, really.
	fd_array[5] = fs_open(fs, "");
	ASSERT_LT(fd_array[5], 0);
	score += 20;

	// OPEN_FILE 7
	fd_array[5] = fs_open(fs, "/");
	ASSERT_LT(fd_array[5], 0);
	fd_array[5] = fs_open(fs, filenames[1]);
	ASSERT_LT(fd_array[5], 0);


	// OPEN_FILE 8
	fd_array[5] = fs_open(fs, filenames[6]);
	ASSERT_LT(fd_array[5], 0);

	// OPEN_FILE 9
	// In case I'm leaking descriptors, wipe them all
	fs_unmount(fs);
	fs = fs_mount(test_fname);
	ASSERT_NE(fs, nullptr);
	for (int i = 0; i < 256; ++i) {
		fd_array[i] = fs_open(fs, filenames[0]);
	}
	int err = fs_open(fs, filenames[0]);
	ASSERT_LT(err, 0);
	fs_unmount(fs);
	score += 20;
}

/*
   int fs_get_dir(const FS *const fs, const char *const fname, dir_rec_t *const records)
   1. Normal, root I guess?
   2. Normal, subdir somewhere
   3. Normal, empty dir
   4. Error, bad path
   5. Error, NULL fname
   6. Error, NULL fs
   7. Error, not a directory
 */

TEST(f_tests, get_dir) 
{
	vector<const char *> fnames
	{
		"/file", "/folder", "/folder/with_file", "/folder/with_folder", "/DOESNOTEXIST", "/file/BAD_REQUEST",
			"/DOESNOTEXIST/with_file", "/folder/with_file/bad_req", "folder/missing_slash", "/folder/new_folder/",
			"/folder/withwaytoolongfilenamethattakesupmorespacethanitshould and yet was not enough so I had to add "
				"more/bad_req",
			"/folder/withfilethatiswayyyyytoolongwhydoyoumakefilesthataretoobigEXACT!", "/", "/mystery_file"
	};
	const char *test_fname = "f_tests.FS";
	ASSERT_EQ(system("cp c_tests.FS f_tests.FS"), 0);
	FS *fs = fs_mount(test_fname);
	ASSERT_NE(fs, nullptr);

	// FS_GET_DIR 1
	dyn_array_t *record_results = fs_get_dir(fs, "/");
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "file"));
	ASSERT_TRUE(find_in_directory(record_results, "folder"));
	ASSERT_EQ(dyn_array_size(record_results), 2);
	dyn_array_destroy(record_results);


	// FS_GET_DIR 2
	record_results = fs_get_dir(fs, fnames[1]);
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "with_file"));
	ASSERT_TRUE(find_in_directory(record_results, "with_folder"));
	ASSERT_EQ(dyn_array_size(record_results), 2);
	dyn_array_destroy(record_results);
	score += 10;

	// FS_GET_DIR 3
	record_results = fs_get_dir(fs, fnames[3]);
	ASSERT_NE(record_results, nullptr);
	ASSERT_EQ(dyn_array_size(record_results), 0);
	dyn_array_destroy(record_results);


	// FS_GET_DIR 4
	record_results = fs_get_dir(fs, fnames[9]);
	ASSERT_EQ(record_results, nullptr);
	score += 10;

	// FS_GET_DIR 5
	record_results = fs_get_dir(fs, NULL);
	ASSERT_EQ(record_results, nullptr);


	// FS_GET_DIR 6
	record_results = fs_get_dir(NULL, fnames[3]);
	ASSERT_EQ(record_results, nullptr);


	// FS_GET_DIR 7
	record_results = fs_get_dir(fs, fnames[0]);
	ASSERT_EQ(record_results, nullptr);
	fs_unmount(fs);
	score += 10;
}

/*
   ssize_t fs_write(FS *fs, int fd, const void *src, size_t nbyte);
   1. Normal, 0 size to < 1 block
   2. Normal, < 1 block to next
   3. Normal, 0 size to 1 block
   4. Normal, 1 block to next
   5. Normal, 1 block to partial
   6. Normal, direct -> indirect
   7. Normal, indirect -> dbl_indirect
   8. Normal, full file (run out of blocks before max file size :/ )
   9. Error, file full/blocks full (also test fs_create 13)
   10. Error, fs NULL
   11. Error, data NULL
   12. Error, nbyte 0 (not an error...? Bad parameters? Hmm.)
   13. Error, bad fd
 */
TEST(d_tests, write_file_simple) 
{
	vector<const char *> fnames{"/file_a", "/file_b", "/file_c", "/file_d"};
	const char *test_fname = "d_tests_normal.FS";
	FS *fs = fs_format(test_fname);
	ASSERT_NE(fs, nullptr);

	uint8_t three_a[BLOCK_SIZE_BYTES];
	memset(three_a, 0x33, 333);
	memset(three_a + 333, 0xAA, BLOCK_SIZE_BYTES-333);
	// 333 0x33, rest is 0xAA
	// I really wish there was a "const but wait like 2 sec I need to write something complex"
	// (actually you can kinda do that with memset and pointer voodoo)

	uint8_t two_nine[BLOCK_SIZE_BYTES];
	memset(two_nine, 0x22, 222);
	memset(two_nine + 222, 0x99, BLOCK_SIZE_BYTES-222);
	// Figure out the pattern yet?

	uint8_t large_eight_five_b_seven[BLOCK_SIZE_BYTES * 3];
	memset(large_eight_five_b_seven, 0x88, 888);
	memset(large_eight_five_b_seven + 888, 0x55, 555);
	memset(large_eight_five_b_seven + 555 + 888, 0xBB, 1111);
	memset(large_eight_five_b_seven + 555 + 1111 + 888, 0x77, BLOCK_SIZE_BYTES*3 - 888 - 555 - 1111);
	ASSERT_EQ(fs_create(fs, fnames[0], FS_REGULAR), 0);

	int fd_array[5] = {-1};  // wonderful arbitrary number
	fd_array[0] = fs_open(fs, fnames[0]);
	ASSERT_GE(fd_array[0], 0);
	ASSERT_EQ(fs_create(fs, fnames[1], FS_REGULAR), 0);
	fd_array[1] = fs_open(fs, fnames[1]);
	ASSERT_GE(fd_array[1], 0);
	ASSERT_EQ(fs_create(fs, fnames[2], FS_REGULAR), 0);
	fd_array[2] = fs_open(fs, fnames[2]);
	ASSERT_GE(fd_array[2], 0);
	score += 12;

	// Alrighty, time to get some work done.
	// FS_WRITE 1
	ASSERT_EQ(fs_write(fs, fd_array[0], three_a, BLOCK_SIZE_BYTES-256), BLOCK_SIZE_BYTES-256);

	// FS_WRITE 2
	ASSERT_EQ(fs_write(fs, fd_array[0], large_eight_five_b_seven, 256), 256);

	// FS_WRITE 3
	ASSERT_EQ(fs_write(fs, fd_array[1], two_nine, BLOCK_SIZE_BYTES), BLOCK_SIZE_BYTES);

	// FS_WRITE 4
	ASSERT_EQ(fs_write(fs, fd_array[1], two_nine, BLOCK_SIZE_BYTES), BLOCK_SIZE_BYTES);

	// FS_WRITE 5
	ASSERT_EQ(fs_write(fs, fd_array[2], large_eight_five_b_seven, BLOCK_SIZE_BYTES*2+128), BLOCK_SIZE_BYTES*2+128);

	// I'll do the breakage tests now, move the big writes somewhere else
	// 2. Normal, attempt to use after closing, assert failure **
	// 3. Normal, multiple opens, close does not affect the others **
	// FS_WRITE 10
	ASSERT_LT(fs_write(NULL, fd_array[2], three_a, 999), 0);

	// FS_WRITE 11
	ASSERT_LT(fs_write(fs, fd_array[2], NULL, 999), 0);

	// Can't validate that it didn't mess up the R/W position :/
	// FS_WRITE 12
	ASSERT_EQ(fs_write(fs, fd_array[2], three_a, 0), 0);

	// FS_WRITE 13
	ASSERT_LT(fs_write(fs, 90, three_a, 12), 0);
	ASSERT_LT(fs_write(fs, -90, three_a, 12), 0);

	// FS_CLOSE 2
	ASSERT_EQ(fs_close(fs, fd_array[0]), 0);
	ASSERT_LT(fs_write(fs, fd_array[0], three_a, 700), 0);

	// FS_CLOSE 3
	fd_array[0] = fs_open(fs, fnames[1]);
	ASSERT_GE(fd_array[0], 0);
	// fd 0 and 1 point to same file
	ASSERT_EQ(fs_close(fs, fd_array[0]), 0);
	ASSERT_EQ(fs_write(fs, fd_array[1], three_a, BLOCK_SIZE_BYTES), BLOCK_SIZE_BYTES);

	// I'm going to unmount without closing.
	fs_unmount(fs);
}

#ifdef HARDWRITETESTS_FS2020
TEST(d_tests, write_file_fill) 
{
	// Still gotta test write 6,7,8,9
	vector<const char *> fnames{"/file_a", "/file_b", "/file_c", "/file_d"};
	const char *test_fname = "d_tests_full.FS";
	FS *fs = fs_format(test_fname);
	ASSERT_NE(fs, nullptr);
	uint8_t large_eight_five_b_seven[BLOCK_SIZE_BYTES * 3];
	memset(large_eight_five_b_seven, 0x88, 888);
	memset(large_eight_five_b_seven + 888, 0x55, 555);
	memset(large_eight_five_b_seven + 555 + 888, 0xBB, 1111);
	memset(large_eight_five_b_seven + 555 + 1111 + 888, 0x77, BLOCK_SIZE_BYTES*3 - 888 - 555 - 1111);
	ASSERT_EQ(fs_create(fs, fnames[0], FS_REGULAR), 0);
	int fd = fs_open(fs, fnames[0]);
	ASSERT_GE(fd, 0);


	// total blocks			65536
	// eaten by free bitmap	2
	// eaten by overhead		5
	// one data block for root dir		1
	// direct inodes	will eat		6
	// indirect inodes	eat		1 + 2048
	// how many left for dbindirect inodes?
	// 65536 - 2 - 5 - 1 - 6 - (1+2048) = 63473
	// Okay, the dbindirect block eats one more for the addressing block,
	// 63473 - 1 = 63472


	// FS_WRITE 6
	// direct/indirect transition is easy, write 6 blocks then one more
	size_t blocks = 1;	// 1st block taken by root dir
	ASSERT_EQ(fs_write(fs, fd, large_eight_five_b_seven, BLOCK_SIZE_BYTES * 3), BLOCK_SIZE_BYTES * 3);
	blocks += 3;
	ASSERT_EQ(fs_write(fs, fd, large_eight_five_b_seven, BLOCK_SIZE_BYTES * 3), BLOCK_SIZE_BYTES * 3);
	blocks += 3;

	blocks += 1;		// a block taken by indirect pointer 

	// FS_WRITE 7
	// fill up the indirect pointers
	for (int i = 0; i < 1024; i++) 
	{
		ASSERT_EQ(fs_write(fs, fd, large_eight_five_b_seven, BLOCK_SIZE_BYTES * 2), BLOCK_SIZE_BYTES * 2);
	}
	blocks += 2048;

	// +2 because dbl indirect and the indirect	
	blocks += 2;

	uint8_t *giant_data_chunk = new (std::nothrow) uint8_t[BLOCK_SIZE_BYTES * 2048]; // an entire indirect block
	ASSERT_NE(giant_data_chunk, nullptr);
	memset(giant_data_chunk, 0x6E, BLOCK_SIZE_BYTES * 2048);

	// now try to fill up all of the rest blocks
	for (int i = 0; i < 30; ++i, blocks += 2049) {
		ASSERT_EQ(fs_write(fs, fd, giant_data_chunk, BLOCK_SIZE_BYTES * 2048), BLOCK_SIZE_BYTES * 2048);
	}
	// Down to the last few blocks now
	// Gonna try and write more than is left, because you should cut it off when you get to the end, not just die.
	// According to my investigation, there's 2008 blocks left
	ASSERT_EQ(fs_write(fs, fd, giant_data_chunk, BLOCK_SIZE_BYTES * 2048), BLOCK_SIZE_BYTES * 2008);
	blocks += 2008;
	delete[] giant_data_chunk;
	// While I'm at it...
	// FS_CREATE 21
	ASSERT_EQ(fs_create(fs, fnames[1], FS_DIRECTORY), 0);
	// And might as well check this
	ASSERT_LT(fs_create(fs, fnames[1], FS_REGULAR), 0);
	ASSERT_EQ(fs_create(fs, fnames[2], FS_REGULAR), 0);
	fs_unmount(fs);
	score += 15;
}
#endif

#ifdef FS_REMOVE_TESTS
/* 
   int fs_remove(FS *fs, const char *path);
   1. Normal, file at root
   2. Normal, file in subdirectory
   3. Normal, directory in subdir, empty directory
   4. Normal, file in double indirects somewhere (use full fs file from write_file?)
   5. Error, directory with contents
   6. Error, file does not exist
   7. Error, Root (also empty)
   8. Error, NULL fs
   9. Error, NULL fname
   10. Error, Empty fname (same as file does not exist?)
 */
TEST(e_tests, remove_file) {
	vector<const char *> b_fnames{
		"/file", "/folder", "/folder/with_file", "/folder/with_folder", "/DOESNOTEXIST", "/file/BAD_REQUEST",
			"/DOESNOTEXIST/with_file", "/folder/with_file/bad_req", "folder/missing_slash", "/folder/new_folder/",
			"/folder/withwaytoolongfilenamethattakesupmorespacethanitshould and yet was not enough so I had to add "
				"more/bad_req",
			"/folder/withfilethatiswayyyyytoolongwhydoyoumakefilesthataretoobigEXACT!", "/", "/mystery_file"};
	vector<const char *> a_fnames{"/file_a", "/file_b", "/file_c", "/file_d"};
	const char *(test_fname[2]) = {"e_tests_a.FS", "e_tests_b.FS"};
	ASSERT_EQ(system("cp d_tests_normal.FS e_tests_a.FS"), 0);
	ASSERT_EQ(system("cp c_tests.FS e_tests_b.FS"), 0);
	FS *fs = fs_mount(test_fname[1]);
	ASSERT_NE(fs, nullptr);
	// FS_REMOVE 10
	ASSERT_LT(fs_remove(fs, ""), 0);

	// FS_REMOVE 2
	ASSERT_EQ(fs_remove(fs, b_fnames[2]), 0);

	// FS_REMOVE 5
	ASSERT_LT(fs_remove(fs, b_fnames[1]), 0);
	ASSERT_EQ(fs_remove(fs, b_fnames[3]), 0);

	// FS_REMOVE 3
	ASSERT_EQ(fs_remove(fs, b_fnames[1]), 0);
	fs_unmount(fs);

	fs = fs_mount(test_fname[0]);
	ASSERT_NE(fs, nullptr);
	// FS_REMOVE 1
	ASSERT_EQ(fs_remove(fs, a_fnames[1]), 0);

	// FS_REMOVE 4
	ASSERT_EQ(fs_remove(fs, a_fnames[0]), 0);

	// FS_REMOVE 6
	ASSERT_LT(fs_remove(fs, a_fnames[3]), 0);

	// FS_REMOVE 7
	ASSERT_LT(fs_remove(fs, "/"), 0);

	// FS_REMOVE 8
	ASSERT_LT(fs_remove(NULL, a_fnames[1]), 0);

	// FS_REMOVE 9
	ASSERT_LT(fs_remove(fs, NULL), 0);
	fs_unmount(fs);
	score += 15;
}
#endif

#if FS_SEEK_TESTS
/*
   off_t fs_seek(FS *fs, int fd, off_t offset, seek_t whence)
   1. Normal, wherever, really - make sure it doesn't change a second fd to the file
   2. Normal, seek past beginning - resulting location unspecified by our api, can't really test?
   3. Normal, seek past end - resulting location unspecified by our api, can't really test?
   4. Error, FS null
   5. Error, fd invalid
   6. Error, whence not a valid value
 */
TEST(g_tests, seek) {
	vector<const char *> fnames{"/file_a", "/file_b", "/file_c", "/file_d"};
	const char *test_fname = "g_tests.FS";
	ASSERT_EQ(system("cp d_tests_normal.FS g_tests.FS"), 0);
	FS *fs = fs_mount(test_fname);
	ASSERT_NE(fs, nullptr);
	int fd_one = fs_open(fs, fnames[0]);
	ASSERT_GE(fd_one, 0);
	int fd_two = fs_open(fs, fnames[0]);
	ASSERT_GE(fd_two, 0);
	// While we're at it, make sure they default to 0
	int position = fs_seek(fs, fd_one, 0, FS_SEEK_CUR);
	ASSERT_EQ(position, 0);
	// FS_SEEK 1
	position = fs_seek(fs, fd_one, 1023, FS_SEEK_CUR);
	ASSERT_EQ(position, 1023);
	position = fs_seek(fs, fd_one, 12, FS_SEEK_SET);
	ASSERT_EQ(position, 12);
	// FS_SEEK 2
	position = fs_seek(fs, fd_one, -50, FS_SEEK_CUR);
	ASSERT_EQ(position, 0);
	// FS_SEEK 3
	//position = fs_seek(fs, fd_one, 298675309, FS_SEEK_CUR);
	//ASSERT_EQ(position, 268296192);

	// while we're at it, make sure seek didn't break the other one
	position = fs_seek(fs, fd_two, 0, FS_SEEK_CUR);
	ASSERT_EQ(position, 0);
	// FS_SEEK 4
	position = fs_seek(NULL, fd_one, 12, FS_SEEK_SET);
	ASSERT_LT(position, 0);
	// FS_SEEK 5
	position = fs_seek(fs, 98, 12, FS_SEEK_SET);
	ASSERT_LT(position, 0);
	// FS_SEEK 6
	position = fs_seek(fs, fd_one, 12, (seek_t) 8458);
	ASSERT_LT(position, 0);
	fs_unmount(fs);
	score += 13;
}
#endif

/*
   ssize_t fs_read(FS *fs, int fd, void *dst, size_t nbyte);
   1. Normal, begin to < 1 block
   2. Normal, < 1 block to part of next
   3. Normal, whole block
   4. Normal, multiple blocks
   5. Normal, direct->indirect transition
   6. Normal, indirect->dbl_indirect transition
   7. Normal, double indirect indirect transition
   8. Error, NULL fs
   9. Error, NULL data
   10. Normal, nbyte 0
   11. Normal, at EOF
 */
TEST(h_tests, read) 
{
	vector<const char *> fnames{"/file_a", "/file_b", "/file_c", "/file_d"};
	const char *test_fname = "g_tests.FS";
	ASSERT_EQ(system("cp d_tests_normal.FS g_tests.FS"), 0);
    uint8_t three_a[BLOCK_SIZE_BYTES];
    memset(three_a, 0x33, 333);
    memset(three_a + 333, 0xAA, BLOCK_SIZE_BYTES-333);
	uint8_t six_e[3072];
	memset(six_e, 0x6E, 3072);
	uint8_t large_eight_five_b_seven[BLOCK_SIZE_BYTES * 3];
	memset(large_eight_five_b_seven, 0x88, 888);
	memset(large_eight_five_b_seven + 888, 0x55, 555);
	memset(large_eight_five_b_seven + 555 + 888, 0xBB, 1111);
	memset(large_eight_five_b_seven + 555 + 1111 + 888, 0x77, 518);
	FS *fs = fs_mount(test_fname);
	ASSERT_NE(fs, nullptr);
	int fd = fs_open(fs, fnames[0]);
	ASSERT_GE(fd, 0);
	uint8_t write_space[4096] = {0};

	// FS_READ 1
	ssize_t nbyte = fs_read(fs, fd, &write_space, 333);
	ASSERT_EQ(nbyte, 333);
	ASSERT_EQ(memcmp(write_space, three_a, 333), 0);

	// FS_READ 2
	nbyte = fs_read(fs, fd, &write_space, 1000);
	ASSERT_EQ(nbyte, 1000);
	ASSERT_EQ(memcmp(write_space, three_a+333, 1000), 0);

	// FS_READ 3
#ifdef FS_SEEK_TESTS
	ASSERT_EQ(fs_seek(fs, fd, 0, FS_SEEK_SET), 0);
#else
    fs_close(fs,fd);
    fd=fs_open(fs, fnames[0]);
#endif
	nbyte = fs_read(fs, fd, &write_space, BLOCK_SIZE_BYTES-256);
	ASSERT_EQ(nbyte, BLOCK_SIZE_BYTES-256);
	ASSERT_EQ(memcmp(write_space, three_a, BLOCK_SIZE_BYTES-256), 0);

	// FS_READ 4
	nbyte = fs_read(fs, fd, &write_space, 256);
	ASSERT_EQ(nbyte, 256);
	ASSERT_EQ(memcmp(write_space, large_eight_five_b_seven, 256), 0);

#ifdef FS_SEEK_TESTS
	// FS_READ 5
	ASSERT_EQ(fs_seek(fs, fd, 6144, FS_SEEK_SET), 6144);
	nbyte = fs_read(fs, fd, &write_space, 2048);
	ASSERT_EQ(nbyte, 2048);
	ASSERT_EQ(memcmp(write_space, large_eight_five_b_seven, 2048), 0);
	// FS_READ 6
	ASSERT_EQ(fs_seek(fs, fd, (512 + 6) * BLOCK_SIZE_BYTES, FS_SEEK_SET), (512 + 6) * BLOCK_SIZE_BYTES);
	nbyte = fs_read(fs, fd, &write_space, 3000);
	ASSERT_EQ(nbyte, 3000);	
	ASSERT_EQ(memcmp(write_space, six_e, 3000), 0);
	score += 15;
	// FS_READ 7					64990 = 513 * 126 + 1 + 351		careful about the calculations here!
	ASSERT_EQ(fs_seek(fs, fd, (6 + 512 + 126 * 512 + 349) * BLOCK_SIZE_BYTES, FS_SEEK_SET), (6 + 512 + 126 * 512 + 349) * BLOCK_SIZE_BYTES);
	nbyte = fs_read(fs, fd, &write_space, 2048);
	ASSERT_EQ(nbyte, 2048);
	ASSERT_EQ(memcmp(write_space, six_e, 2048), 0);
	// FS_READ 8
	nbyte = fs_read(NULL, fd, write_space, BLOCK_SIZE_BYTES);
	ASSERT_LT(nbyte, 0);
	// did you break the descriptor position?
	ASSERT_EQ(fs_seek(fs, fd, 0, FS_SEEK_CUR), (6 + 512 + 126 * 512 + 351) * BLOCK_SIZE_BYTES);
	// FS_READ 9
	nbyte = fs_read(fs, fd, NULL, BLOCK_SIZE_BYTES);
	ASSERT_LT(nbyte, 0);
	// did you break the descriptor position?
	ASSERT_EQ(fs_seek(fs, fd, 0, FS_SEEK_CUR), (6 + 512 + 126 * 512 + 351) * BLOCK_SIZE_BYTES);
	// FS_READ 10
	nbyte = fs_read(fs, fd, write_space, 0);
	ASSERT_EQ(nbyte, 0);
	ASSERT_EQ(fs_seek(fs, fd, 0, FS_SEEK_CUR), (6 + 512 + 126 * 512 + 351) * BLOCK_SIZE_BYTES);
	// FS_READ 11
	ASSERT_EQ(fs_seek(fs, fd, 1, FS_SEEK_CUR), 66950144);
	ASSERT_EQ(fs_seek(fs, fd, -500, FS_SEEK_END), 66949644);
	nbyte = fs_read(fs, fd, write_space, BLOCK_SIZE_BYTES);
	ASSERT_EQ(nbyte, 500);
	ASSERT_EQ(memcmp(write_space, six_e, 500), 0);
	// did you mess up the position?
	ASSERT_EQ(fs_seek(fs, fd, 0, FS_SEEK_CUR), 66950144);
#endif

	fs_unmount(fs);
	score += 20;
}

#ifdef FS_MOVE_TESTS
/*
   int fs_move(FS *fs, const char *src, const char *dst);
   1. Normal, file, one dir to another (check descriptor)
   2. Normal, directory
   3. Normal, Rename of file where the directory is full
   4. Error, dst exists
   5. Error, dst parent does not exist
   6. Error, dst parent full
   7. Error, src does not exist
   8. ?????, src = dst
   9. Error, FS null
   10. Error, src null
   11. Error, src is root
   12. Error, dst NULL
   13. Error, dst root?
   14. Error, Directory into itself
 */
TEST(i_tests, move) {
	vector<const char *> fnames{
		"/file", "/folder", "/folder/with_file", "/folder/with_folder", "/DOESNOTEXIST", "/file/BAD_REQUEST",
			"/DOESNOTEXIST/with_file", "/folder/with_file/bad_req", "folder/missing_slash", "/folder/new_folder/",
			"/folder/withwaytoolongfilenamethattakesupmorespacethanitshould and yet was not enough so I had to add "
				"more/bad_req",
			"/folder/withfilethatiswayyyyytoolongwhydoyoumakefilesthataretoobigEXACT!", "/", "/mystery_file"};
	const char *test_fname = "g_tests.FS";
	ASSERT_EQ(system("cp c_tests.FS g_tests.FS"), 0);
	FS *fs = fs_mount(test_fname);
	ASSERT_NE(fs, nullptr);
	dyn_array_t *record_results = NULL;
	// FS_MOVE 2
	ASSERT_EQ(fs_move(fs, fnames[3], "/folder2"), 0);
	record_results = fs_get_dir(fs, "/");
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "file"));
	ASSERT_TRUE(find_in_directory(record_results, "folder"));
	ASSERT_TRUE(find_in_directory(record_results, "folder2"));
	ASSERT_EQ(dyn_array_size(record_results), 3);
	dyn_array_destroy(record_results);
	record_results = fs_get_dir(fs, fnames[1]);
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "with_file"));
	//    ASSERT_TRUE(find_in_directory(record_results, "new_location"));
	ASSERT_EQ(dyn_array_size(record_results), 1);
	dyn_array_destroy(record_results);
	int fd = fs_open(fs, fnames[0]);
	// FS_MOVE 1    
	ASSERT_EQ(fs_move(fs, fnames[0], "/folder/new_location"), 0);
	record_results = fs_get_dir(fs, fnames[1]);
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "with_file"));
	ASSERT_TRUE(find_in_directory(record_results, "new_location"));
	ASSERT_EQ(dyn_array_size(record_results), 2);
	dyn_array_destroy(record_results);
	record_results = fs_get_dir(fs, "/");
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "folder"));
	ASSERT_TRUE(find_in_directory(record_results, "folder2"));
	ASSERT_EQ(dyn_array_size(record_results), 2);
	dyn_array_destroy(record_results);
	// Descriptor still functional?
	ASSERT_EQ(fs_write(fs, fd, test_fname, 14), 14);
	// FS_MOVE 4
	ASSERT_LT(fs_move(fs, "/folder/new_location", fnames[1]), 0);
	record_results = fs_get_dir(fs, "/");
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "folder"));
	ASSERT_TRUE(find_in_directory(record_results, "folder2"));
	ASSERT_EQ(dyn_array_size(record_results), 2);
	dyn_array_destroy(record_results);
	record_results = fs_get_dir(fs, fnames[1]);
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "with_file"));
	ASSERT_TRUE(find_in_directory(record_results, "new_location"));
	ASSERT_EQ(dyn_array_size(record_results), 2);
	dyn_array_destroy(record_results);
	// FS_MOVE 5
	ASSERT_LT(fs_move(fs, "/folder/new_location", "/folder/noooope/new_location"), 0);
	record_results = fs_get_dir(fs, "/");
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "folder"));
	ASSERT_TRUE(find_in_directory(record_results, "folder2"));
	ASSERT_EQ(dyn_array_size(record_results), 2);
	dyn_array_destroy(record_results);
	record_results = fs_get_dir(fs, fnames[1]);
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "with_file"));
	ASSERT_TRUE(find_in_directory(record_results, "new_location"));
	ASSERT_EQ(dyn_array_size(record_results), 2);
	dyn_array_destroy(record_results);
	// FS_MOVE 7
	ASSERT_LT(fs_move(fs, "/folder/DNE", "/folder/also_DNE"), 0);
	record_results = fs_get_dir(fs, "/");
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "folder"));
	ASSERT_TRUE(find_in_directory(record_results, "folder2"));
	ASSERT_EQ(dyn_array_size(record_results), 2);
	dyn_array_destroy(record_results);
	record_results = fs_get_dir(fs, fnames[1]);
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "with_file"));
	ASSERT_TRUE(find_in_directory(record_results, "new_location"));
	ASSERT_EQ(dyn_array_size(record_results), 2);
	dyn_array_destroy(record_results);
	// FS_MOVE 8
	// this one is just weird, so... skipping
	// FS_MOVE 9
	ASSERT_LT(fs_move(NULL, "/folder/DNE", "/folder/also_DNE"), 0);
	// FS_MOVE 10
	ASSERT_LT(fs_move(fs, NULL, "/folder/also_DNE"), 0);
	// FS_MOVE 11
	ASSERT_LT(fs_move(fs, "/", "/folder/root_maybe"), 0);
	// FS_MOVE 12
	ASSERT_LT(fs_move(fs, "/folder/new_location", NULL), 0);
	// FS_MOVE 13
	ASSERT_LT(fs_move(fs, "/folder/new_location", "/"), 0);
	// FS_MOVE 14
	ASSERT_LT(fs_move(fs,"/folder","/folder/oh_no"),0);
	// Things still working after all that ?
	record_results = fs_get_dir(fs, "/");
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "folder"));
	ASSERT_TRUE(find_in_directory(record_results, "folder2"));
	ASSERT_EQ(dyn_array_size(record_results), 2);
	dyn_array_destroy(record_results);
	record_results = fs_get_dir(fs, fnames[1]);
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "with_file"));
	ASSERT_TRUE(find_in_directory(record_results, "new_location"));
	ASSERT_EQ(dyn_array_size(record_results), 2);
	dyn_array_destroy(record_results);
	fs_unmount(fs);
	score += 15;
}
#endif

#ifdef FS_LINK_TESTS
/*
   int fs_link(FS *fs, const char *src, const char *dst);
   1. Normal, file, make a link next to it
   2. Normal, directory, link next to it
   3. Normal, OH BOY, directory will contain itself (check that /folder/itself/itself/itself/itself/with_file exists)
   4. Normal, file, wite to hardlink, read the new data from fd to original file
   5. Normal, file, delete hardlinked file, make sure original still works
   6. Normal, directory, delete a hardlink directory that has contents!
   7. Error, dst exists
   8. Error, dst parent does not exist
   9. Error, dst parent full
   10. Error, src does not exist
   11. Error, FS null
   12. Error, src null
   13. Error, dst null
   14. Error, dst root
 */
TEST(j_tests, link) {
	const char * test_fname = "j_tests.FS";

	FS * fs = fs_format(test_fname);
	ASSERT_NE(fs, nullptr); // format

	// 1. Normal, file, make a link next to it
	ASSERT_EQ(fs_create(fs, "/file", FS_REGULAR), 0);
	ASSERT_EQ(fs_link(fs, "/file", "/file1"), 0);

	// 2. Normal, directory, link next to it
	ASSERT_EQ(fs_create(fs, "/folder", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_link(fs, "/folder", "/folder0"), 0);

	// 7. Error, dst exists
	ASSERT_LT(fs_link(fs, "/file", "/file1"), 0);

	// 8. Error, dst parent does not exist
	ASSERT_LT(fs_link(fs, "/file", "/NOTEXISTFOLDER/file1"), 0);

	// 9. Error, dst parent full
	ASSERT_EQ(fs_create(fs, "/folder1", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/1", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/2", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/3", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/4", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/5", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/6", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/7", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/8", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/9", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/10", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/11", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/12", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/13", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/14", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/15", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/16", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/17", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/18", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/19", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/20", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/21", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/22", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/23", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/24", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/25", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/26", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/27", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/28", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/29", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/30", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder1/31", FS_DIRECTORY), 0);
	ASSERT_LT(fs_link(fs, "/file", "/folder1/file"), 0);

	// 10. Error, src does not exist
	ASSERT_LT(fs_link(fs, "/NOTEXIST", "/file2"), 0);

	// 11. Error, FS null
	ASSERT_LT(fs_link(nullptr, "/file", "/file2"), 0);

	// 12. Error, src null
	ASSERT_LT(fs_link(fs, nullptr, "/file2"), 0);

	// 13. Error, dst null
	ASSERT_LT(fs_link(fs, "file", nullptr), 0);

	// 14. Error, dst root
	ASSERT_LT(fs_link(fs, "file", "/"), 0);

	//  3. Normal, OH BOY, directory will contain itself (check that /folder/itself/itself/itself/itself/with_file exists)
	ASSERT_EQ(fs_create(fs, "/folder/itself", FS_DIRECTORY), 0);
	ASSERT_EQ(fs_create(fs, "/folder/itself/with_file", FS_REGULAR), 0);
	ASSERT_EQ(fs_link(fs, "/folder/itself", "/folder/itself/itself"), 0);
	dyn_array_t * record_results = fs_get_dir(fs, "/folder/itself/itself/itself/itself");
	ASSERT_NE(record_results, nullptr);
	ASSERT_TRUE(find_in_directory(record_results, "with_file"));
	dyn_array_destroy(record_results);

	//  4. Normal, file, write to hardlink, read the new data from fd to original file
	int fd = fs_open(fs, "/file1"); // "/file1" is a hardlink of "/file"
	ASSERT_GE(fd, 0);
	uint8_t three[BLOCK_SIZE_BYTES]      = {0};
	uint8_t three_test[BLOCK_SIZE_BYTES] = {0};
	memset(three, 0x33, BLOCK_SIZE_BYTES);
	ASSERT_EQ(fs_write(fs, fd, three, BLOCK_SIZE_BYTES), BLOCK_SIZE_BYTES);
	ASSERT_EQ(fs_close(fs, fd), 0);
	fd = fs_open(fs, "/file");
	ASSERT_GE(fd, 0);
	ASSERT_EQ(fs_read(fs, fd, three_test, BLOCK_SIZE_BYTES), BLOCK_SIZE_BYTES);
	ASSERT_EQ(memcmp(three, three_test, BLOCK_SIZE_BYTES), 0);
	ASSERT_EQ(fs_close(fs, fd), 0);


	//  5. Normal, file, delete hardlinked file, make sure original still works
	ASSERT_EQ(fs_remove(fs, "/file1"), 0);
	fd = fs_open(fs, "/file");
	ASSERT_GE(fd, 0);
	memset(three_test, 0, BLOCK_SIZE_BYTES);
	ASSERT_EQ(fs_read(fs, fd, three_test, BLOCK_SIZE_BYTES), BLOCK_SIZE_BYTES);
	ASSERT_EQ(memcmp(three, three_test, BLOCK_SIZE_BYTES), 0);
	ASSERT_EQ(fs_close(fs, fd), 0);

	//  6. Normal, directory, delete a hardlink directory that has contents!
	ASSERT_EQ(fs_link(fs, "/folder1", "/folder2"), 0); // "/folder1" is full dir
	ASSERT_LT(fs_remove(fs, "/folder2"), 0);           // 不能被删除， hardlink 对应的是 inode， 不应删除空目录

	// Close fs
	fs_unmount(fs);
	score += 20;
}
#endif

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	::testing::AddGlobalTestEnvironment(new GradeEnvironment);
	return RUN_ALL_TESTS();
}
