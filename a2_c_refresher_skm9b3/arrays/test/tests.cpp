#include <stdio.h>
extern "C" {
	#include "../include/arrays.h"
}
#include <gtest/gtest.h>


/*
*  ARRAY COPY UNIT TEST CASES
**/
TEST (array_copy, NullSrc) {
	int dst[5] = {0};
	EXPECT_EQ(false,array_copy(NULL,dst,sizeof(int),5));
}

TEST (array_copy, NullDst) {
	int src[5] = {0};
	EXPECT_EQ(false,array_copy(src,NULL,sizeof(int),5));
}

TEST (array_copy, ZeroSize) {
	int src[5] = {0};
	int dst[5] = {1,2,3,4,5};
	EXPECT_EQ(false,array_copy(src,dst,0,5));
}

TEST (array_copy, ZeroCount) {
	int src[5] = {0};
	int dst[5] = {1,2,3,4,5};
	EXPECT_EQ(false,array_copy(src,dst,sizeof(int),0));
}

TEST (array_copy, GoodCopy) {
	int src[5] = {1,2,3,4,5};
	int dst[5] = {0};
	ASSERT_EQ(true,array_copy(src,dst,sizeof(int),5));

	size_t i = 0;
	for ( ; i < 5; ++i) {
		ASSERT_EQ(dst[i], i + 1);
	}
}

/*
* ARRAY IS EQUAL UNIT TEST CASES
**/

TEST (array_is_equal, NullSrc) {
	int dst[5] = {0};
	EXPECT_EQ(false,array_is_equal(NULL,dst,sizeof(int),5));
}

TEST (array_is_equal, NullDst) {
	int src[5] = {0};
	EXPECT_EQ(false,array_is_equal(src,NULL,sizeof(int),5));
}

TEST (array_is_equal, ZeroSize) {
	int src[5] = {0};
	int dst[5] = {1,2,3,4,5};
	EXPECT_EQ(false,array_is_equal(src,dst,0,5));
}

TEST (array_is_equal, ZeroCount) {
	int src[5] = {0};
	int dst[5] = {1,2,3,4,5};
	EXPECT_EQ(false,array_is_equal(src,dst,sizeof(int),0));
}

TEST (array_is_equal, SameFrontDifferentRest) {
	int src[5] = {1,4,5,6,7};
	int dst[5] = {1,2,3,4,5};
	EXPECT_EQ(false,array_is_equal(src,dst,sizeof(int),5));
}

TEST (array_is_equal, SameArray) {
	int src[5] = {1,2,3,4,5};
	int dst[5] = {1,2,3,4,5};
	EXPECT_EQ(true,array_is_equal(src,dst,sizeof(int),5));
}

/*
* ARRAY LOCATE UNIT TESTS
**/

TEST (array_locate, NullSrc) {
	int target = 0;
	EXPECT_EQ(-1,array_locate(NULL,&target,sizeof(int),5));
}

TEST (array_locate, NullTarget) {
	int src[5] = {0};
	EXPECT_EQ(-1,array_locate(src,NULL,sizeof(int),5));
}

TEST (array_locate, ZeroSize) {
	int src[5] = {0};
	int target = 0;
	EXPECT_EQ(-1,array_locate(src,&target,0,5));
}

TEST (array_locate, ZeroCount) {
	int src[5] = {0};
	int target = 0;
	EXPECT_EQ(-1,array_locate(src,&target,sizeof(int),0));
}


TEST (array_locate, NotFoundTarget) {
	int src[5] = {1,2,4,5,6};
	int target = 3;
	EXPECT_EQ(-1,array_locate(src,&target,sizeof(int),5));
}

TEST (array_locate, FoundTarget) {
	int src[5] = {1,2,4,5,6};
	int target = 4;
	EXPECT_EQ(2,array_locate(src,&target,sizeof(int),5));
}

/*
* ARRAY SERIALIZE UNIT TESTS
**/


TEST (array_serialize, NullData) {
	const char* dst_file = "testing_file.dat";
	EXPECT_EQ(false,array_serialize(NULL,dst_file,sizeof(int),5));
}

TEST (array_serialize, NullFileName) {
	int data[5] = {0};
	EXPECT_EQ(false,array_serialize(data,NULL,sizeof(int),5));
}

TEST (array_serialize, ZeroElemSize) {
	const char* dst_file = "testing_file.dat";
	int data[5] = {0};
	EXPECT_EQ(false,array_serialize(data,dst_file,0,5));
}

TEST (array_serialize, ZeroCount) {
	const char* dst_file = "testing_file.dat";
	int data[5] = {0};
	EXPECT_EQ(false,array_serialize(data,dst_file,sizeof(int),0));
}

TEST (array_serialize, TrickyBadFileName) {
	const char* dst_file = "";
	int data[5] = {0};
	EXPECT_EQ(false,array_serialize(data,dst_file,sizeof(int),5));
}

TEST (array_serialize, TrickyBadFileNameNewline) {
	const char* dst_file = "\n";
	int data[5] = {0};
	EXPECT_EQ(false,array_serialize(data,dst_file,sizeof(int),5));
}

TEST (array_serialize, GoodFileName) {
	const char* src_file = "actual_file.txt";
	int data[5] = {1,2,3,4,5};
	ASSERT_EQ(true,array_serialize(data,src_file,sizeof(int),5));

	size_t numberOfBytesToRead = sizeof(int) * 5;
	FILE* fp = fopen(src_file, "r");
	if (!fp) {
		printf("FAILURE TO OPEN FILE\n");
	}
	char* res = new char[numberOfBytesToRead];
	ASSERT_EQ(fread(res,1,numberOfBytesToRead,fp), numberOfBytesToRead);
	ASSERT_EQ(0,memcmp(res,(char*)data,numberOfBytesToRead));
	fclose(fp);
	delete[] res;
}

/*
* ARRAY DESERIALIZE UNIT TESTS
*/

TEST (array_deserialize, NullData) {
	const char* src_file = "testing_file.dat";
	EXPECT_EQ(false,array_deserialize(src_file,NULL,sizeof(int),5));
}

TEST (array_deserialize, NullFileName) {
	int data[5] = {0};
	EXPECT_EQ(false,array_deserialize(NULL,data,sizeof(int),5));
}

TEST (array_deserialize, ZeroElemSize) {
	const char* src_file = "testing_file.dat";
	int data[5] = {0};
	EXPECT_EQ(false,array_deserialize(src_file,data,0,5));
}

TEST (array_deserialize, ZeroCount) {
	const char* src_file = "testing_file.dat";
	int data[5] = {0};
	EXPECT_EQ(false,array_deserialize(src_file,data,sizeof(int),0));
}

TEST (array_deserialize, TrickyBadFileName) {
	const char* src_file = "";
	int data[5] = {0};
	EXPECT_EQ(false,array_deserialize(src_file,data,sizeof(int),5));
}

TEST (array_deserialize, TrickyBadFileNameNewline) {
	const char* src_file = "\n";
	int data[5] = {0};
	EXPECT_EQ(false,array_deserialize(src_file,data,sizeof(int),5));
}

TEST (array_deserialize, GoodFileName) {
	const char* src_file = "actual_file.txt";
	int data[5] = {0};
	const int expected[5] = {1,2,3,4,5};
	ASSERT_EQ(true,array_deserialize(src_file,data,sizeof(int),5));
	size_t numberOfBytesToRead = sizeof(int) * 5;
	ASSERT_EQ(0,memcmp(expected,data,numberOfBytesToRead));
}



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






