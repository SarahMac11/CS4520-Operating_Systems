#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../include/arrays.h"

// LOOK INTO MEMCPY, MEMCMP, FREAD, and FWRITE
//
// void *memcpy(void *dest, const void * src, size_t n)
//      dest = ptr to dest array to be copied, type-cast to ptr of type void*
//      src = ptr to source of data to be copied, type-cast to ptr of type void*
//      n = # of bytes to be copied
//
// int memcmp(const void *str1, const void *str2, size_t n)
//      str1 = ptr to block of memory
//      str2 = ptr to block of memory
//      n = # of bytes to be compared
//
//      if Return value < 0 then it indicates str1 is less than str2
//      if Return value > 0 then it indicates str2 is less than str1
//      if Return value = 0 then it indicates str1 is equal to str2
//
// size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
//      ptr = ptr to array of elements to be written
//      size = size in bytes of each element to be written
//      nmemb = # of elements, each with a size bytes
//      stream = ptr to to a FILE object that specifies an output stream

bool array_copy(const void *src, void *dst, const size_t elem_size, const size_t elem_count)
{
    // error check each parameter
    if(src && dst && elem_size && elem_count) {
        // src is copied into dest and n (# of bytes) is elem count * size  
        memcpy(dst, src, elem_size * elem_count);
        return true;        // successful operation
    }
    return false;           // else false
}

bool array_is_equal(const void *data_one, void *data_two, const size_t elem_size, const size_t elem_count)
{
    // error check 
    if(data_one && data_two && elem_size && elem_count) {
        // if arrays are equal, memcmp = 0
        if(memcmp(data_one, data_two, elem_size * elem_count) == 0)
            return true;
    }
    return false;
}

ssize_t array_locate(const void *data, const void *target, const size_t elem_size, const size_t elem_count)
{
    // error check
    if(data && target && elem_size && elem_count) {
        // loop through to find target and index location
        for(int i = 0; i < elem_count; i++) {
            // compare elements to see if data matchest the target
            if(memcmp(data, target, elem_size) == 0) 
                return i;   // return index to located target
            data = data + elem_size;    // data is the data + the element size
        }
    }
    return -1;              // else return -1 for failure
}

bool array_serialize(const void *src_data, const char *dst_file, const size_t elem_size, const size_t elem_count)
{
    // error check 
    if(src_data && dst_file && elem_size && elem_count) { 
        FILE *fp;       // binary file 
        fp = fopen(dst_file, "w"); 
        char *tricky = "\n";    // trickey chars like new line 
        // strstr searches for the needle (tricky char(s)) in a haystack (dst_file) 
        if(fp != NULL && strstr(dst_file, tricky) == NULL) { 
            // check total # elements successfully returned match element count 
            if(fwrite(src_data, elem_size, elem_count, fp) == elem_count) { 
                fclose(fp);     // close file 
                return true; 
            } 
        } 
    } 
    return false; 
}

bool array_deserialize(const char *src_file, void *dst_data, const size_t elem_size, const size_t elem_count)
{
    // error check
    if(src_file && dst_data && elem_size && elem_count) {
        FILE *fp;
        fp = fopen(src_file, "r");
        char *tricky = "\n";
        if(fp != NULL && strstr(src_file, tricky) == NULL) {
            if(fread(dst_data, elem_size, elem_count, fp) == elem_count) {
                fclose(fp);
                return true;
            }
        }
    }
    return false;
}

