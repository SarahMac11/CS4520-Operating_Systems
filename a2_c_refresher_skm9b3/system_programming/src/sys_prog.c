#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../include/sys_prog.h"

// LOOK INTO OPEN, READ, WRITE, CLOSE, FSTAT/STAT, LSEEK
//
// int open (const char* Path, int flags [, int mode ]); 
//      Path = path to file to be opened (absolute)
//      flags = how to use (read, write, etc.)
//
//      Returns file descriptor used, -1 upon failure
//
// size_t read (int fd, void* buf, size_t cnt);  
//      fd = file descripter
//      buf = buffer to read data from
//      cnt = length of buffer
//
//      return # of bytes read on success
//      return 0 on reaching end of file
//      return -1 on error
//
// size_t write (int fd, void* buf, size_t cnt); 
//      fd = file descripter
//      buf = buffer to write data to
//      cnt = length of buffer
//
//      return # of bytes written on success
//      return 0 on reaching end of file
//      return -1 on error
//
// int close(int fd); 
//      fd = file descripter
//
//      returns 0 on success and -1 on error
//
// int fstat(int fd, struct stat *buf);
//      fd = file desc
//      buf = where data about file will be stored
//
//      returns -1 on failure
//
// off_t lseek(int fd, off_t offset, int whence);
//      fd = file desc
//      offset = offset of pointer (measured in bytes)
//      whence = method where offset is to be interpreted
//
//      returns offset of the pointer in bytes from BEGINNING of file
//
// GOOGLE FOR ENDIANESS HELP

bool bulk_read(const char *input_filename, void *dst, const size_t offset, const size_t dst_size)
{
    // error check params and file name
    if(input_filename && dst && offset <= dst_size && dst_size >= 1 && (strcmp(input_filename, "\n") != 0) && (strcmp(input_filename, "\0")) && (strcmp(input_filename, "") != 0)) {
        // open the file with read only
        int fd = open(input_filename, O_RDONLY);
        if(fd == -1)    // if failed to open file
            return false;
        else {
            // seach for part of file that should be read
            lseek(fd, offset, SEEK_SET);  
            if(read(fd, dst, dst_size) == dst_size) {
                if(close(fd) != -1)     // if closes properly
                    return true;
            }
        }    
    }
    return false; 
}

bool bulk_write(const void *src, const char *output_filename, const size_t offset, const size_t src_size)
{
    if(src && output_filename && src_size >= 1 && (strcmp(output_filename, "\n") != 0) && (strcmp(output_filename, "\0") != 0) && (strcmp(output_filename, "") != 0)) {
        // open file with write only
        int fd = open(output_filename, O_WRONLY);
        if(fd == -1) 
            return false;
        else {
            lseek(fd, offset, SEEK_SET);
            if(write(fd, src, src_size) == src_size) {
                if(close(fd) != -1) 
                    return true;
            }
        }
    }
    return false;
}


bool file_stat(const char *query_filename, struct stat *metadata)
{
    if(query_filename && metadata && (strcmp(query_filename, "\n") != 0) && (strcmp(query_filename, "\0") != 0) && (strcmp(query_filename, "") != 0)) {
        int fd = open(query_filename, O_RDONLY);
        int meta = fstat(fd, metadata);
        printf("pass names ");
        close(fd);
        if(meta == 0) {
            return true;
        }
    }
    return false;
}

bool endianess_converter(uint32_t *src_data, uint32_t *dst_data, const size_t src_count)
{
    if(src_data && dst_data && src_count > 0) {
        int i;
        // Swap endian (big to little) or (little to big)
        uint32_t a, b, c, d;
        uint32_t res;
        for(i = 0; i < src_count; ++i) {
            a = (src_data[i] & 0x000000ff) << 24u;
            b = (src_data[i] & 0x0000ff00) << 8u;
            c = (src_data[i] & 0x00ff0000) >> 8u;
            d = (src_data[i] & 0xff000000) >> 24u;
            res = a | b | c | d;        // OR
            dst_data[i] = res;
        }
        return true;
    }
    return false;
}

