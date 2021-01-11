#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

#define MAX_NAME_LEN 28

typedef struct {
	uint32_t age;
	uint8_t name[MAX_NAME_LEN];
}Record_t;

// Allocates and zeros all the fields in the array of records
// \param records a NULL Record_t pointer that will be allocated to the size
//	of num_records
// \param num_records the number of records that will be created
// \return 0 for successful allocation and zero initialization
// else returns -1 for bad parameters, -2 for unsuccessful allocation
int create_blank_records(Record_t **records, const size_t num_records);

// Reads an array of structures from a binary file
// \param input_filename the file that contains the binary array of structures
// \param records an allocated Records array
// \param num_records the number of records to read from the file
// \return 0 for successfully filled records
// else returns -1 for bad parameters, -2 for could not open file, -3 for read any error that prevent readings
int read_records(const char *input_filename, Record_t *records, const size_t num_records);


// Create an alloacted and filled record with the passed name and age
// \param new_record a NULL poiner Record_t struct
// \param name the name to be stored in the new_record
// 	restriction: name must be less than 50 characters
// \param age the age to be stored in the new_record
//	restriction: age must be between 1 and 200
// \return 0 for successful allocation and zero initialization
// else returns -1 for bad parameters, -2 for unsuccessful allocation
int create_record(Record_t **new_record, const char* name, int age);

#ifdef __cplusplus
    }
#endif

#endif //ERROR_HANDLING_H