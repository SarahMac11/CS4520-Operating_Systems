#include <stdio.h>
#include <stdlib.h>
extern "C" {
	#include "../include/sstring.h"
}

#include <gtest/gtest.h>

/*
*  STRING VALID UNIT TEST CASES
**/
TEST (string_valid, NullString) {
	EXPECT_EQ(false,string_valid(NULL,5));
}

TEST (string_valid, ZeroCount) {
	const char *string = "matt";
	EXPECT_EQ(false,string_valid(string,0));
}

TEST (string_valid, EmptyString) {
	const char *string = "";
	EXPECT_EQ(true,string_valid(string,1));
}

TEST (string_valid, NotValidString) {
	const char string[4] = {'m','a','t','t'};
	EXPECT_EQ(false,string_valid(string,4));
}

TEST (string_valid, ValidString) {
	const char *string = "matt";
	EXPECT_EQ(true,string_valid(string,5));
}

/*
* STRING DUPLICATE UNIT TEST CASES
**/

TEST (string_duplicate, NullString) {
	EXPECT_EQ(NULL,string_duplicate(NULL,5));
}

TEST (string_duplicate, ZeroCount) {
	const char *string = "matt";
	EXPECT_EQ(NULL,string_duplicate(string,0));
}

TEST (string_duplicate, ValidStringDuplicate) {
	const char *string = "matt";
	char* res = string_duplicate(string,5);
	EXPECT_NE(res,(char*)NULL);
	if (res) {
	  ASSERT_EQ(0, memcmp(string, res, 5 * sizeof(char)));
	}
	free(res);
}


/*
* STRING EQUAL UNIT TEST CASES
**/

TEST (string_equal, NullStringA) {
	const char* str_b = "test";
	EXPECT_EQ(false,string_equal(NULL,str_b,5));
}

TEST (string_equal, NullStringB) {
	const char *str_a = "test";
	EXPECT_EQ(false,string_equal(str_a,NULL,5));
}

TEST (string_equal, ZeroCount) {
	const char *str_a = "test";
	const char *str_b = "test";
	EXPECT_EQ(false,string_equal(str_a,str_b,0));
}

TEST (string_equal, TwoEmptyString) {
	const char *str_a = "";
	const char *str_b = "";
	EXPECT_EQ(true,string_equal(str_a,str_b,1));
}

TEST (string_equal, NotEqualStrings) {
	const char *str_a = "";
	const char *str_b = "test";
	EXPECT_EQ(false,string_equal(str_a,str_b,5));
}

TEST (string_equal, EqualStrings) {
	const char *str_a = "test";
	const char *str_b = "test";
	EXPECT_EQ(true,string_equal(str_a,str_b,5));
}

/*
* STRING LENGTH UNIT TEST CASES
**/

TEST (string_length, NullString) {
	EXPECT_EQ(-1,string_length(NULL,5));
}

TEST (string_length, ZeroCount) {
	const char *str = "test";
	EXPECT_EQ(-1,string_length(str,0));
}

TEST (string_length, HandlesIncorrectCountPassed) {
	const char *str = "test";
	EXPECT_EQ(4,string_length(str,6));
}

TEST (string_length, CorrectString) {
	const char *str = "test";
	EXPECT_EQ(4,string_length(str,5));
}

TEST (string_length, MessyString) {
	const char str[6] = {'m','a','t','\0','t','\0'};
	EXPECT_EQ(3,string_length(str,6));
}

/*
* STRING TOKENIZER UNIT TEST CASES
*/

TEST (string_tokenize, NullString) {
	char *tokens[1];
	EXPECT_EQ(0,string_tokenize(NULL,",",5,tokens,5,1));
}

TEST (string_tokenize, NullDelims) {
	const char *str = "matt";
	char *tokens[1];
	const char *delims = NULL;
	EXPECT_EQ(0,string_tokenize(str,delims,5,tokens,5,1));
}

TEST (string_tokenize, ZeroInputStringLength) {
	const char *str = "matt";
	const char *delims = ",";
	char *tokens[1];
	EXPECT_EQ(0,string_tokenize(str,delims,0,tokens,5,1));
}

TEST (string_tokenize, NullTokens) {
	const char *str = "matt";
	const char *delims = ",";
	size_t length = 1;
	EXPECT_EQ(0,string_tokenize(str,delims,length,NULL,5,1));
}

TEST (string_tokenize, ZeroTokenRequests) {
	const char *str = "matt";
	const char *delims = ",";
	size_t length = 1;
	char *tokens[1];
	EXPECT_EQ(0,string_tokenize(str,delims,length,tokens,5,0));
}

TEST (string_tokenize, ZeroTokenLength) {
	const char *str = "matt";
	const char *delims = ",";
	size_t length = 1;
	char *tokens[1];
	EXPECT_EQ(0,string_tokenize(str,delims,length,tokens,0,1));
}

TEST (string_tokenize, InadequatelyAllocatedTokens) {
	const char *str = "matt,is,cool";
	const char *delims = ",";
	size_t length = 13;
	char *tokens[3];
	tokens[0] = new char[10];
	tokens[1] = NULL;
	tokens[2] = new char[10];

	EXPECT_EQ(-1,string_tokenize(str,delims,length,tokens,10,3));

	delete[] tokens[0];
	delete[] tokens[2];

}

TEST (string_tokenize, TokensParsed) {
	const char *str = "matt,is,cool";
	const char *delims = ",";
	size_t length = 13;
	char *tokens[3];
	tokens[0] = new char[10];
	tokens[1] = new char[10];
	tokens[2] = new char[10];

	EXPECT_EQ(3,string_tokenize(str,delims,length,tokens,10,3));

	ASSERT_EQ(0,strncmp("matt",tokens[0],4));
	ASSERT_EQ(0,strncmp("is",tokens[1],2));
	ASSERT_EQ(0,strncmp("cool",tokens[2],4));

	delete[] tokens[0];
	delete[] tokens[1];
	delete[] tokens[2];

}


/*
* STRING TO INT UNIT TEST CASES
**/

TEST (string_to_int, NullString) {
	int value = 0;
	EXPECT_EQ(false,string_to_int(NULL,&value));
}

TEST (string_to_int, ValueNull) {
	const char *str = "1";
	int *val = NULL;
	EXPECT_EQ(false,string_to_int(str,val));
}

TEST (string_to_int, SimpleCorrectConversion) {
	const char *str = "1024";
	int value = 0;
	EXPECT_EQ(true,string_to_int(str,&value));
	EXPECT_EQ(1024,value);
}

TEST (string_to_int, CannotConvertString) {
	const char *str = "10000000000000000000";
	int value = 0;
	EXPECT_EQ(false,string_to_int(str,&value));
	EXPECT_EQ(0,value);

}


TEST (string_to_int, MessyCorrectString) {
	const char *str = "1024 1048";
	int value = 0;
	EXPECT_EQ(true,string_to_int(str,&value));
	EXPECT_EQ(1024,value);

}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


