#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/structures.h"

#include <gtest/gtest.h>

TEST(struct_size, EQUAL){
  EXPECT_EQ(sizeof(apple_t),sizeof(orange_t));
	EXPECT_EQ(sizeof(orange_t),sizeof(fruit_t));
}

TEST(compare_structs, Equal) {
	sample_t sample1;
	sample1.a = 1;
	sample1.b = 2;
	sample1.c = 'X';

	sample_t sample2;
	sample2.a = 1;
	sample2.b = 2;
	sample2.c = 'X';
    EXPECT_EQ(1, compare_structs(&sample1,&sample2));
}

TEST(compare_structs, NotEqual) {
	sample_t sample1;
	sample1.a = 1;
	sample1.b = 1;
	sample1.c = 'X';

	sample_t sample2;
	sample2.a = 0;
	sample2.b = 1;
	sample2.c = 'X';
    EXPECT_EQ(0, compare_structs(&sample1,&sample2));
}

TEST(fruit_count, correct){
	size_t size = rand() % 41 + 10;
	fruit_t* fruit = (fruit_t*)malloc(sizeof(fruit_t)*size);
	int expect_apples = (rand() % size) + 1;
	int expect_oranges = size - expect_apples;
	initialize_array(fruit, expect_apples, expect_oranges);
	int apples = 0;
	int oranges = 0;
	int result = sort_fruit(fruit,&apples,&oranges,size);
	EXPECT_EQ(expect_apples,apples);
	EXPECT_EQ(expect_oranges,oranges);
	EXPECT_EQ(size,result);
	free(fruit);
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}



