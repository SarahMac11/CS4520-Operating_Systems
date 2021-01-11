#ifndef ARRAYS_H
#define ARRAYS_H
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

// HINT: MEMCPY and MEMCMP are useful functions to use

// Copy the contents from source array into destination array
// \param src the array the will be copied into the destination array
// \param dst the array that will be copied from the source array
// \param elem_size the number of bytes each array element uses
// \param elem_count the number of elements in the source array
// return true if operation was successful, else false
bool array_copy(const void *src, void *dst, const size_t elem_size, const size_t elem_count);

// Compares if two arrays contain the same contents
// \param array_a the array that will be compared against array_b
// \param array_b the array that will be compared against array_a
// \param elem_size the number of bytes each array element uses in array_a
// \param elem_count the number of elements in array_a
// return true if operation was successful, else false
bool array_is_equal(const void *array_a, void *array_b, const size_t elem_size, const size_t elem_count);

// Writes an array into a binary file
// \param src_data the array the will be wrote into the destination file
// \param dst_file the file that will contain the wrote src_data
// \param elem_size the number of bytes each array element uses
// \param elem_count the number of elements in the source array
// return true if operation was successful, else false
bool array_serialize(const void *src_data, const char *dst_file, const size_t elem_size, const size_t elem_count);

// Reads an array from a binary file
// \param src_file the source file that contains the array to be read into the destination array
// \param dst_data the array that will contain the data stored inthe source file
// \param elem_size the number of bytes each array element uses of the destination array
// \param elem_count the number of elements in the destination array
// return true if operation was successful, else false
bool array_deserialize(const char *src_file, void *dst_data, const size_t elem_size, const size_t elem_count);

// Attempts to locate the target from an array
// \param data the data that may contain the target
// \param target the target that may be in the data
// \param elem_size the number of bytes each array element uses and same as the target
// \param elem_count the number of elements in the data array
// returns an index to the located target, else return -1 for failure
ssize_t array_locate(const void *data, const void *target, const size_t elem_size, const size_t elem_count);

#endif
