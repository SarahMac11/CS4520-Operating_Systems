#ifndef SYS_PROG_H
#define SYS_PROG_H
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>

//
// NOTE THE FILE I/O MUST USE OPEN, READ, WRITE, CLOSE, SEEK, STAT with file descriptors (NO FILE*)
// Make sure to uint8_t or uint32_t, you are dealing with system dependent sizes

// Read contents from the passed into an destination
// \param input_filename the file containing the data to be copied into the destination
// \param dst the variable that will be contain the copied contents from the file
// \param offset the starting location in the file, how many bytes inside the file I start reading
// \param dst_size the total number of bytes the destination variable contains
// return true if operation was successful, else false
bool bulk_read(const char *input_filename, void *dst, const size_t offset, const size_t dst_size);

// Writes contents from the data source into the outputfile
// \param src the source of the data to be wrote to the output_filename
// \param output_filename the file that is used for writing
// \param offset the starting location in the file, how many bytes inside the file I start writing
// \param src_size the total number of bytes the src variable contains
// return true if operation was successful, else false
bool bulk_write(const void *src, const char *output_filename, const size_t offset, const size_t src_size);

// Returns the file metadata given a filename
// \param query_filename the filename that will be queried for stats
// \param metadata the buffer that contains the metadata of the queried filename
// return true if operation was successful, else false
bool file_stat(const char *query_filename, struct stat *metadata);

// Converts the endianess of the source data contents before storing into the dst_data.
// The passed source data bits are swapped from little to big endian and vice versa.
// \param src_data the source data that contains content to be stored into the destination
// \param dst_data the destination that stores src data
// \param src_count the number of src_data elements
// \return true if successful, else false for failure
bool endianess_converter(uint32_t *src_data, uint32_t *dst_data, const size_t src_count);



#endif
