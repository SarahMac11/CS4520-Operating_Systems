#include <stdio.h>
#include <stdint.h>
#include <math.h>

extern "C" {
	#include "../include/bitmap.h"
}

#include <gtest/gtest.h>

/*
 *	BITMAP CREATE UNIT TEST CASES
 **/
 /*
 This is a bad test for multiple reasons
 TEST(bitmap_create, NegBitVal) {
 	EXPECT_EQ(NULL, bitmap_create(-1));
 }
 */

 TEST(bitmap_create, ZeroBitVal) {
 	EXPECT_EQ(nullptr, bitmap_create(0));
 }

 /*
 Also a bad test, at least on 32-bit systems
 TEST(bitmap_create, TooLargeBitVal){
 	EXPECT_EQ(NULL, bitmap_create(SIZE_MAX));
 }
 */

 TEST(bitmap_create, GoodCreateA) {
 	bitmap_t *bitmap_A;
 	size_t test_bit_count = 64;
 	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A, nullptr);
 	ASSERT_EQ(test_bit_count/8, bitmap_A->byte_count);
 	if (bitmap_A) {
	  bitmap_destroy(bitmap_A);
	}
 }
 // Handling the issue that you must create enough bytes for requested bits.
 // You may have to add more bits to allow for a bitmap size that is not
 // nicely divisable by 8 bits
 TEST(bitmap_create, GoodCreateB) {
 	bitmap_t *bitmap_A;
 	size_t test_bit_count = 371;
 	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A, nullptr);
 	ASSERT_EQ(ceil(test_bit_count/8.0), bitmap_A->byte_count);
 	if (bitmap_A) {
	  bitmap_destroy(bitmap_A);
	}
 }

 /*
 * BITMAP DESTROY UNIT TEST CASES
 **/

 TEST(bitmap_destroy, BadBitmapObj) {
 	EXPECT_EQ(false, bitmap_destroy(NULL));
 }

 TEST(bitmap_destroy, GoodDestr) {
 	bitmap_t *bitmap_A = bitmap_create(8);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
 	EXPECT_EQ(true, bitmap_destroy(bitmap_A));
 }

/*
 *	BITMAP SET UNIT TEST CASES
 **/

 TEST(bitmap_set, BadBitmapObj){
  	bitmap_t *bitmap_A;
  	bitmap_A = bitmap_create(22);
	ASSERT_NE(bitmap_A, nullptr);
  	EXPECT_EQ(false, bitmap_set(NULL, 8));
  	if (bitmap_A)
		bitmap_destroy(bitmap_A);
  }

 TEST(bitmap_set, TooLargeBitVal) {
 	bitmap_t *bitmap_A;
  	size_t test_bit_count = 22;
  	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
 	EXPECT_EQ(false, bitmap_set(bitmap_A, SIZE_MAX));
 	if (bitmap_A)
	  bitmap_destroy(bitmap_A);
 }

 TEST(bitmap_set, NegBitVal) {
  	bitmap_t *bitmap_A;
  	size_t test_bit_count = 87;
  	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
  	EXPECT_EQ(false, bitmap_set(bitmap_A, -1));
  	if (bitmap_A)
	  bitmap_destroy(bitmap_A);
 }

TEST(bitmap_set, GoodSetA) {
	bitmap_t * bitmap_A;
	size_t test_bit_count = 58, test_byte_count = 8;
	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
	EXPECT_EQ(true, bitmap_set(bitmap_A, 0));
	EXPECT_EQ(true, bitmap_set(bitmap_A, test_bit_count - 1));
	ASSERT_EQ(0x02, bitmap_A->data[test_byte_count -1]);
	ASSERT_EQ(0x01, bitmap_A->data[0]);
	if (bitmap_A)
          bitmap_destroy(bitmap_A);
}

TEST(bitmap_set, GoodSetB) {
	bitmap_t * bitmap_A;
	size_t test_bit_count = 58;
	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
	EXPECT_EQ(true, bitmap_set(bitmap_A, 1));
	EXPECT_EQ(true, bitmap_set(bitmap_A, 8));
	ASSERT_EQ(0x02, bitmap_A->data[0]);
	ASSERT_EQ(0x01, bitmap_A->data[1]);
	EXPECT_EQ(true, bitmap_set(bitmap_A, 2));
	EXPECT_EQ(true, bitmap_set(bitmap_A, 9));
	ASSERT_EQ(0x06, bitmap_A->data[0]);
	ASSERT_EQ(0x03, bitmap_A->data[1]);
	EXPECT_EQ(true, bitmap_set(bitmap_A, 3));
	EXPECT_EQ(true, bitmap_set(bitmap_A, 10));
	ASSERT_EQ(0x0E, bitmap_A->data[0]);
	ASSERT_EQ(0x07, bitmap_A->data[1]);
	EXPECT_EQ(true, bitmap_set(bitmap_A, 4));
	EXPECT_EQ(true, bitmap_set(bitmap_A, 11));
	ASSERT_EQ(0x1E, bitmap_A->data[0]);
	ASSERT_EQ(0x0F, bitmap_A->data[1]);
	if (bitmap_A)
          bitmap_destroy(bitmap_A);
}

/*
 *	BITMAP RESET UNIT TEST CASES
 **/

 TEST(bitmap_reset, NegBitVal) {
 	bitmap_t *bitmap_A;
 	bitmap_A = bitmap_create(12);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
 	EXPECT_EQ(false, bitmap_reset(bitmap_A, -1));
 	bitmap_destroy(bitmap_A);
 }

 TEST(bitmap_reset, BadBitmapObj) {
 	bitmap_t *bitmap_A;
 	bitmap_A = bitmap_create(12);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
 	EXPECT_EQ(false, bitmap_reset(NULL, 2));
 	bitmap_destroy(bitmap_A);
 }

 TEST(bitmap_reset, TooLargeBitVal) {
 	bitmap_t *bitmap_A;
  	size_t test_bit_count = 17;
  	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
 	EXPECT_EQ(false, bitmap_set(bitmap_A, SIZE_MAX));
 	bitmap_destroy(bitmap_A);
 }

TEST(bitmap_reset, GoodResetA) {
	bitmap_t * bitmap_A;
	size_t test_bit_count = 58;
	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
	EXPECT_EQ(true, bitmap_set(bitmap_A, 2));
	EXPECT_EQ(true, bitmap_set(bitmap_A, 9));
	ASSERT_EQ(0x04, bitmap_A->data[0]);
	ASSERT_EQ(0x02, bitmap_A->data[1]);
	EXPECT_EQ(true, bitmap_set(bitmap_A, 3));
	EXPECT_EQ(true, bitmap_set(bitmap_A, 10));
	ASSERT_EQ(0xC, bitmap_A->data[0]);
	ASSERT_EQ(0x06, bitmap_A->data[1]);
	EXPECT_EQ(true, bitmap_set(bitmap_A, 4));
	EXPECT_EQ(true, bitmap_set(bitmap_A, 11));
	ASSERT_EQ(0x1C, bitmap_A->data[0]);
	ASSERT_EQ(0xE, bitmap_A->data[1]);
	EXPECT_EQ(true, bitmap_reset(bitmap_A, 4));
	EXPECT_EQ(true, bitmap_reset(bitmap_A, 11));
	ASSERT_EQ(0xC, bitmap_A->data[0]);
	ASSERT_EQ(0x06, bitmap_A->data[1]);
	EXPECT_EQ(true, bitmap_reset(bitmap_A, 3));
	EXPECT_EQ(true, bitmap_reset(bitmap_A, 10));
	ASSERT_EQ(0x04, bitmap_A->data[0]);
	ASSERT_EQ(0x02, bitmap_A->data[1]);
	EXPECT_EQ(true, bitmap_reset(bitmap_A, 2));
	EXPECT_EQ(true, bitmap_reset(bitmap_A, 9));
	ASSERT_EQ(0x00, bitmap_A->data[0]);
	ASSERT_EQ(0x00, bitmap_A->data[1]);
	bitmap_destroy(bitmap_A);
}

TEST(bitmap_reset, GoodResetB) {
	bitmap_t * bitmap_A;
	size_t test_bit_count = 58, test_byte_count = 8;
	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
	EXPECT_EQ(true, bitmap_set(bitmap_A, 0));
	EXPECT_EQ(true, bitmap_set(bitmap_A, test_bit_count - 1));
	ASSERT_EQ(0x02, bitmap_A->data[test_byte_count -1]);
	ASSERT_EQ(0x01, bitmap_A->data[0]);
	EXPECT_EQ(true, bitmap_reset(bitmap_A, 0));
	EXPECT_EQ(true, bitmap_reset(bitmap_A, test_bit_count - 1));
	ASSERT_EQ(0x00, bitmap_A->data[test_byte_count -1]);
	ASSERT_EQ(0x00, bitmap_A->data[0]);
	bitmap_destroy(bitmap_A);
}

/*
 *	BITMAP TEST UNIT TEST CASES
 **/

 TEST(bitmap_test, NegBitVal) {
 	bitmap_t *bitmap_A;
  	size_t test_bit_count = 61;
  	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
  	EXPECT_EQ(false, bitmap_set(bitmap_A, -1));
  	bitmap_destroy(bitmap_A);
 }

TEST(bitmap_test, BadBitmapObj) {
	bitmap_t *bitmap_A;
  	size_t test_bit_count = 38;
  	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
  	EXPECT_EQ(false, bitmap_set(NULL, 22));
  	bitmap_destroy(bitmap_A);
}

TEST(bitmap_test, TooLargeBitVal) {
	bitmap_t *bitmap_A;
  	size_t test_bit_count = 92;
  	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
  	EXPECT_EQ(false, bitmap_set(bitmap_A, SIZE_MAX));
  	bitmap_destroy(bitmap_A);
}

TEST(bitmap_test, GoodTestA) {
	bitmap_t * bitmap_A;
	size_t test_bit_count = 51;
	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
	EXPECT_EQ(true, bitmap_set(bitmap_A, 1));
	EXPECT_EQ(true, bitmap_set(bitmap_A, 8));
	EXPECT_EQ(true, bitmap_test(bitmap_A, 1));
	EXPECT_EQ(true, bitmap_test(bitmap_A, 8));
	EXPECT_EQ(true, bitmap_reset(bitmap_A, 1));
	EXPECT_EQ(true, bitmap_reset(bitmap_A, 8));
	EXPECT_EQ(false, bitmap_test(bitmap_A, 1));
	EXPECT_EQ(false, bitmap_test(bitmap_A, 8));
	bitmap_destroy(bitmap_A);
}

TEST(bitmap_test, GoodTestB) {
	bitmap_t * bitmap_A;
	size_t test_bit_count = 39;
	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
	EXPECT_EQ(false, bitmap_test(bitmap_A, 22));
	EXPECT_EQ(true, bitmap_set(bitmap_A, 22));
	EXPECT_EQ(true, bitmap_test(bitmap_A, 22));
	EXPECT_EQ(false, bitmap_test(bitmap_A, 13));
	EXPECT_EQ(true, bitmap_set(bitmap_A, 13));
	EXPECT_EQ(true, bitmap_test(bitmap_A, 13));
	bitmap_destroy(bitmap_A);
}

/*
 * BITMAP FIND FIRST SET (BIT) UNIT TEST CASES
 **/

 TEST(bitmap_ffs, BadBitmapObj) {
 	EXPECT_EQ(SIZE_MAX, bitmap_ffs(NULL));
 }

TEST(bitmap_ffs, NoSetBit) {
	bitmap_t *bitmap_A;
	size_t test_bit_count = 72;
	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
    EXPECT_EQ(SIZE_MAX, bitmap_ffs(bitmap_A));
    bitmap_destroy(bitmap_A);
}

TEST(bitmap_ffs, GoodffsA) {
	bitmap_t *bitmap_A;
	size_t test_bit_count = 112;
	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
	EXPECT_EQ(true, bitmap_set(bitmap_A, test_bit_count - 1));
	EXPECT_EQ(test_bit_count - 1, bitmap_ffs(bitmap_A));
	bitmap_destroy(bitmap_A);
}

TEST(bitmap_ffs, GoodffsB) {
	bitmap_t *bitmap_A;
	size_t test_bit_count = 14;
	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
	EXPECT_EQ(SIZE_MAX, bitmap_ffs(bitmap_A));
	EXPECT_EQ(true, bitmap_set(bitmap_A, 11));
	EXPECT_EQ(11, bitmap_ffs(bitmap_A));
	bitmap_destroy(bitmap_A);
}

TEST(bitmap_ffs, GoodffsC) {
	bitmap_t *bitmap_A;
	size_t test_bit_count = 231;
	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
	EXPECT_EQ(true, bitmap_set(bitmap_A, 201));
	EXPECT_EQ(201, bitmap_ffs(bitmap_A));
	bitmap_destroy(bitmap_A);
}

/*
 *	BITMAP FIND FIRST ZERO (BIT) UNIT TEST CASES
 **/

 TEST(bitmap_ffz, BadBitmapObj) {
 	EXPECT_EQ(SIZE_MAX, bitmap_ffz(NULL));
 }

 TEST(bitmap_ffz, NoZeroBit) {
 	bitmap_t *bitmap_A;
	size_t test_bit_count = 41;
	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
	for(size_t i = 0; i < test_bit_count; ++i)
		EXPECT_EQ(true, bitmap_set(bitmap_A, i));
	EXPECT_EQ(SIZE_MAX, bitmap_ffz(bitmap_A));
	bitmap_destroy(bitmap_A);
 }

 TEST(bitmap_ffz, GoodffzA) {
 	bitmap_t *bitmap_A;
	size_t test_bit_count = 83;
	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
	for(size_t i = 0; i < test_bit_count - 1; ++i)
		EXPECT_EQ(true, bitmap_set(bitmap_A, i));
	EXPECT_EQ(test_bit_count - 1, bitmap_ffz(bitmap_A));
	bitmap_destroy(bitmap_A);
 }

 TEST(bitmap_ffz, GoodffzB) {
 	bitmap_t *bitmap_A;
	size_t test_bit_count = 41;
	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
	EXPECT_EQ(0, bitmap_ffz(bitmap_A));
	for(size_t i = 0; i < test_bit_count; ++i) {
		if(i == 29){
			continue;
		}
		else
			EXPECT_EQ(true, bitmap_set(bitmap_A, i));
	}
	EXPECT_EQ(29, bitmap_ffz(bitmap_A));
	bitmap_destroy(bitmap_A);
 }

 TEST(bitmap_ffz, GoodffzC) {
 	bitmap_t *bitmap_A;
	size_t test_bit_count = 413;
	bitmap_A = bitmap_create(test_bit_count);
	ASSERT_NE(bitmap_A,(bitmap_t*)NULL);
	for(size_t i = 0; i < test_bit_count; ++i) {
		if(i == 199)
			continue;
		else
			EXPECT_EQ(true, bitmap_set(bitmap_A, i));
	}
	EXPECT_EQ(199, bitmap_ffz(bitmap_A));
	bitmap_destroy(bitmap_A);
 }

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
