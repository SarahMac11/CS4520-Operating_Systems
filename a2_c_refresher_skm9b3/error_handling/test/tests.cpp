#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdint.h>

#include "../include/error_handling.h"

#include <gtest/gtest.h>

// int create_blank_records(Record_t **records, const size_t num_records)
TEST (create_blank_records, BadParams) 
{
	Record_t * records = NULL;
	size_t num_records = 0;
	EXPECT_EQ(-1,create_blank_records(&records, num_records));

	records = new Record_t;
	num_records = 10;
	EXPECT_EQ(-1,create_blank_records(&records, num_records));
	delete records;
}

TEST (create_blank_records, BadMemoryAllocation) 
{
	Record_t * records = NULL;
	size_t num_records = -1;
	EXPECT_EQ(-2,create_blank_records(&records, num_records));

}

TEST (create_blank_records, GoodAllocation) 
{
	Record_t * records = NULL;
	size_t num_records = 10;
	EXPECT_EQ(0,create_blank_records(&records, num_records));
	bool res = records ? true : false;
	EXPECT_EQ(true,res);
	free(records);

}



//int read_records(const char *input_filename, Record_t *records, const size_t num_records)
TEST (read_records, BadParams) 
{

	// bad input filename -- null case
	const char *input_filename1 = NULL;
	Record_t  records[10];
	size_t num_records = 10;
	EXPECT_EQ(-1,read_records(input_filename1,&records[0],num_records));

	// bad records -- null records
	const char *input_filename3 = "records.bin";
	EXPECT_EQ(-1,read_records(input_filename3,NULL,num_records));

	// bad num_records -- zero records to read
	EXPECT_EQ(-1,read_records(input_filename3,&records[0],0));
}

TEST (read_records, BadFilenameGiven) 
{
	const char *input_filename = "wrong_file.bin";
	Record_t  records[10];
	size_t num_records = 10;
	EXPECT_EQ(-2,read_records(input_filename,&records[0],num_records));
}

TEST (read_records, BadReadRequest) 
{
	const char *input_filename = "records.bin";
	Record_t records[10];
	// bad num records -- too large
	EXPECT_EQ(-3,read_records(input_filename,&records[0],200));
}

TEST (read_records, GoodRead) 
{
	const char *input_filename = "records.bin";
	Record_t records[3];
	size_t num_records = 3;
	Record_t exp_records[3] = {
		{23,"foo"},
		{5,"fred"},
		{18,"sue"}
	};
	// bad num records -- too large
	EXPECT_EQ(0,read_records(input_filename, &records[0], num_records));
	for (int i = 0; i < 3; ++i) {
		EXPECT_EQ(exp_records[i].age,records[i].age);
		EXPECT_EQ(0,strncmp((char*) exp_records[i].name,(char*)records[i].name,strlen((char*)exp_records[i].name)));
	}
}

//int create_record(Record_t **new_record, const char* name, int age)
//(!new_record || (*new_record) || !name || name[0] == '\0' || name[0] == '\n'
//	|| strlen(name) < MAX_NAME_LEN - 1 || age < 0 || age > 200

TEST (create_record, BadParams) 
{
	// bad age test 1
	Record_t *record = NULL;
	const char *name = "matthew";
	int age = -1;
	EXPECT_EQ(-1,create_record(&record, name, age));

	// bad age test 2
	record = NULL;
	age = 0;
	EXPECT_EQ(-1,create_record(&record, name, age));
	// bad age test 3
	record = NULL;
	age = 201;
	EXPECT_EQ(-1,create_record(&record, name, age));

	// bad name test 1 -- null name
	age = 10;
	EXPECT_EQ(-1,create_record(&record, NULL, age));

	// bad name test 2 -- newline name
	age = 10;
	name = "\n";
	EXPECT_EQ(-1,create_record(&record, name, age));

	// bad name test 3 -- too long name
	age = 10;
	const char *name2 = "thelongestnamethatwillnotworkbutactuallywasn'tlogenoughbecausenobodycandoanythingright";
	EXPECT_EQ(-1,create_record(&record, name2, age));

	// bad record -- allocated record
	record = new Record_t;
	age = 10;
	EXPECT_EQ(-1,create_record(&record, name, age));
	delete record;

}

TEST (create_record, GoodAllocation) 
{
	Record_t * record = NULL;
	const char *name = "matthew";
	int age = 10;
	EXPECT_EQ(0,create_record(&record, name, age));
	bool res = record ? true : false;
	EXPECT_EQ(true,res);
	free(record);

}

int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
