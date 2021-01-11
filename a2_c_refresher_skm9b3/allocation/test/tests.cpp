#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../include/allocation.h"
#include <gtest/gtest.h>

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();

}

TEST(calloc_memory, ZeroBytes) {
	char* allocated = (char*) allocate_array(sizeof(char), 0,1);
	ASSERT_EQ((char*)NULL, allocated);
}
TEST(calloc_memory, NegativeBytes) {
	char* allocated = (char*) allocate_array(sizeof(char),-1,1);
	ASSERT_EQ((char*)NULL, allocated);
}
TEST(calloc_memory, PositiveBytes) {
	char* allocated = (char*)allocate_array(sizeof(char), BUFSIZ,1);
	ASSERT_NE((char*)NULL, allocated);
	free(allocated);
}
TEST(malloc_memory, ZeroBytes) {
	char* allocated = (char*) allocate_array(sizeof(char), 0,0);
	ASSERT_EQ((char*)NULL, allocated);
}
TEST(malloc_memory, NegativeBytes) {
	char* allocated = (char*)allocate_array(sizeof(char), -1,0);
	ASSERT_EQ((char*)NULL, allocated);
}
TEST(malloc_memory, PositiveBytes) {
	char* allocated = (char*)allocate_array(sizeof(char), BUFSIZ,0);
	ASSERT_NE((char*)NULL, allocated);
	free(allocated);
}
TEST(reallocate, ZeroBytes) {
	char* allocated = (char*) allocate_array(sizeof(char), BUFSIZ,1);
	ASSERT_NE((char*)NULL, allocated);
	char* reallocated = (char*) reallocate_array(allocated,0);
	EXPECT_EQ((char*)NULL,reallocated);
	//Calling realloc with not-null pointer and 0 bytes is a wrapper to free.
	//free(allocated);
}
TEST(reallocate, NegativeBytes) {
	char* allocated = (char*) allocate_array(sizeof(char), BUFSIZ,1);
	ASSERT_NE((char*)NULL, allocated);
	char* reallocated = (char*)reallocate_array(allocated,-1);
	EXPECT_EQ((char*)NULL,reallocated);
	free(allocated);
}
TEST(reallocate, PositiveBytes) {
	char* allocated = (char*) allocate_array(sizeof(char), BUFSIZ,1);
	ASSERT_NE((char*)NULL, allocated);
	char* reallocated = (char*)reallocate_array(allocated,BUFSIZ*2);
	EXPECT_NE((char*)NULL,reallocated);
	free(reallocated);
}

TEST(deallocate, ValidPointer) {
	char* allocated = (char*)allocate_array(sizeof(char), BUFSIZ,1);
	deallocate_array((void**)&allocated);
	EXPECT_EQ((char*)NULL,(char*)allocated);
}
TEST(deallocate, NullPointer) {
	char* allocated = NULL;
	deallocate_array((void**)&allocated);
	EXPECT_EQ((char*)NULL,(char*)allocated);
}
TEST(readline_to_buffer,ValidFile){

	char filename[] = "test.txt";
	int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
	write(fd,"Test",4);
	close(fd);
	char* buffer = read_line_to_buffer(filename);
	ASSERT_NE(buffer,(char*)NULL);
	EXPECT_TRUE(strcmp(buffer,"Test") == 0);
	free(buffer);
}
