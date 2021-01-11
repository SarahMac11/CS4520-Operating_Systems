#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

extern "C" {
	#include "../include/sys_prog.h"
}

#include <gtest/gtest.h>

/*
* BULK READ TEST CASES
**/


TEST (bulk_read, NullInputFilename) {
	const char *input_filename = NULL;
	char dst[20];
	const size_t offset = 0;
	const size_t dst_size = 20;
	bool res = bulk_read(input_filename,dst,offset,dst_size);
	EXPECT_EQ(res,false);
}

TEST (bulk_read, BadInputFilename) {
	const char *bad_filename1 = "\n";
	char dst[20];
	const size_t offset = 0;
	const size_t dst_size = 20;
	bool res = bulk_read(bad_filename1,dst,offset,dst_size);
	EXPECT_EQ(res,false);

	const char* bad_filename2 = "\0";
	res = bulk_read(bad_filename2,dst,offset,dst_size);
	EXPECT_EQ(res,false);
}

TEST (bulk_read, NonExistingInputFilename) {
	const char *input_filename = "dat.binary";
	char dst[20];
	const size_t offset = 0;
	const size_t dst_size = 20;
	bool res = bulk_read(input_filename,dst,offset,dst_size);
	EXPECT_EQ(res,false);

}

TEST (bulk_read, NullDst) {
	const char *input_filename = "dat.bin";
	char *dst = NULL;
	const size_t offset = 0;
	const size_t dst_size = 20;
	bool res = bulk_read(input_filename,dst,offset,dst_size);
	EXPECT_EQ(res,false);

}

TEST (bulk_read, BadOffset) {
	const char *input_filename = "dat.bin";
	char dst[20];
	const size_t offset = 21;
	const size_t dst_size = 20;
	bool res = bulk_read(input_filename,dst,offset,dst_size);
	EXPECT_EQ(res,false);
}

TEST (bulk_read, ZeroDstSize) {
	const char *input_filename = "dat.bin";
	char dst[20];
	const size_t offset = 0;
	const size_t dst_size = 0;
	bool res = bulk_read(input_filename,dst,offset,dst_size);
	EXPECT_EQ(res,false);
}

TEST (bulk_read, CorrectFileReadWithZeroOffset) {
	const char *input_filename = "dat.bin";
	char dst[20];
	const size_t offset = 0;
	const size_t dst_size = 20;
	bool res = bulk_read(input_filename,dst,offset,dst_size);
	EXPECT_EQ(res,true);

}

TEST (bulk_read, CorrectFileReadWithMiddleOffset) {
	const char *input_filename = "dat.bin";
	char dst[20];
	const size_t offset = 10;
	const size_t dst_size = 10;
	bool res = bulk_read(input_filename,dst,offset,dst_size);
	EXPECT_EQ(res,true);

}

/*
* BULK WRITE TEST CASES
**/

TEST (bulk_write, NullInputFilename) {
	const char *input_filename = NULL;
	char src[20];
	const size_t offset = 0;
	const size_t src_size = 20;
	bool res = bulk_write(src,input_filename,offset,src_size);
	EXPECT_EQ(res,false);
}

TEST (bulk_write, BadInputFilenames) {
	const char *bad_filename1 = "\n";
	char src[20];
	const size_t offset = 0;
	const size_t src_size = 20;
	bool res = bulk_write(src,bad_filename1,offset,src_size);
	EXPECT_EQ(res,false);

	const char* bad_filename2 = "\0";
	res = bulk_write(src,bad_filename2,offset,src_size);
	EXPECT_EQ(res,false);

	const char* bad_filename3 = "";
	res = bulk_write(src,bad_filename3,offset,src_size);
	EXPECT_EQ(res,false);
}

TEST (bulk_write, NullDst) {
	const char *input_filename = "out.bin";
	char *src = NULL;
	const size_t offset = 0;
	const size_t src_size = 20;
	bool res = bulk_write(src,input_filename,offset,src_size);
	EXPECT_EQ(res,false);

}

TEST (bulk_write, ZeroDstSize) {
	const char *input_filename = "out.bin";
	char src[20];
	const size_t offset = 0;
	const size_t src_size = 0;
	bool res = bulk_write(src,input_filename,offset,src_size);
	EXPECT_EQ(res,false);
}

TEST (bulk_write, CorrectFileReadWithZeroOffset) {
	const char *input_filename = "out.bin";
	char src[20];
	memset(src,98,20);
	const size_t offset = 0;
	const size_t src_size = 20;
	bool res = bulk_write(src,input_filename,offset,src_size);
	EXPECT_EQ(res,true);

}

TEST (bulk_write, CorrectFileReadWithMiddleOffset) {
	const char *input_filename = "out.bin";
	char src[20];
	memset(src,98,20);
	const size_t offset = 10;
	const size_t src_size = 10;
	bool res = bulk_write(src,input_filename,offset,src_size);
	EXPECT_EQ(res,true);

}

TEST (bulk_write, ExpandingOffset) {
	const char *input_filename = "offset_fun.bin";
	char src[20];
	const size_t offset = 21;
	const size_t src_size = 20;
	bool res = bulk_write(src,input_filename,offset,src_size);
	EXPECT_EQ(res,true);
}

/*
* FILE STAT TEST CASES
**/

TEST (file_stat, NullQueryFilename) {
	const char *query_filename = NULL;
	struct stat file_stats;
	bool res = file_stat(query_filename,&file_stats);
	EXPECT_EQ(res,false);
}

TEST (file_stat, NullFileStats) {
	const char *query_filename = "out.bin";
	struct stat *file_stats = NULL;
	bool res = file_stat(query_filename,file_stats);
	EXPECT_EQ(res,false);
}

TEST (file_stat, IncorrectFile) {
	const char *query_filename = "wrong_file.bin";
	struct stat file_stats;
	bool res = file_stat(query_filename,&file_stats);
	EXPECT_EQ(res,false);
}

TEST (file_stat, CorrectFilename) {
	const char *query_filename = "offset_fun.bin";
	struct stat file_stats;
	bool res = file_stat(query_filename,&file_stats);
	EXPECT_EQ(res,true);
	ASSERT_EQ(41,file_stats.st_size);

}

/*
* ENDIANESS CONVERTER TEST CASES
**/

TEST (endianess_converter, NullSrcData) {
	uint32_t *src_data = NULL;
	uint32_t dst_data[20];
	const size_t src_count = 20;
	bool res = endianess_converter(src_data,dst_data,src_count);
	EXPECT_EQ(res,false);

}

TEST (endianess_converter, NullDstData) {
	uint32_t src_data[20];
	uint32_t *dst_data = NULL;
	const size_t src_count = 20;
	bool res = endianess_converter(src_data,dst_data,src_count);
	EXPECT_EQ(res,false);
}

TEST (endianess_converter, ZeroSrcCount) {
	uint32_t src_data[20];
	uint32_t dst_data[20];
	const size_t src_count = 0;
	bool res = endianess_converter(src_data,dst_data,src_count) ;
	EXPECT_EQ(res,false);
}


TEST (endianess_converter, GoodConversionToLinuxEndianess) {

	uint32_t src_data[20];
	uint32_t dst_data[20];
	uint32_t exp_data[20];
	const size_t src_count = 20;
	for (size_t i = 0; i < src_count; ++i) {
		exp_data[i] = i + 1;
	}

	int fd = open("WrongEndianData.bin",O_RDONLY);
	read(fd,&src_data,20 * sizeof(uint32_t));
	close(fd);

	bool res = endianess_converter(src_data,dst_data,src_count) ;
	ASSERT_EQ(res,true);

	for (size_t i = 0; i < src_count; ++i) {
		EXPECT_EQ(exp_data[i],dst_data[i]);
	}


}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






