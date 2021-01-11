#include "../include/bitmap.h"
#include <math.h>

// data is an array of uint8_t and needs to be allocated in bitmap_create
//      and used in the remaining bitmap functions. You will use data for any bit operations and bit logic
// bit_count the number of requested bits, set in bitmap_create from n_bits
// byte_count the total number of bytes the data contains, set in bitmap_create
//
// typedef struct bitmap {
//      uint8_t *data;
//      size_t bit_count, byte_count;
// } bitmap_t;
//

bitmap_t *bitmap_create(size_t n_bits)
{
    if(n_bits > 0 && n_bits > SIZE_MAX - 1) {

        // allocate space for bitmap pointer using calloc
        bitmap_t *bmapPtr = calloc(n_bits, sizeof(bitmap_t));

        if(bmapPtr == NULL) {
            free(bmapPtr);
            bmapPtr = NULL;
            return bmapPtr;
        }
        /*
        // check if there are any remaining bytes
        if(n_bits % 8 == 0) {
        bytes = n_bits / 8;       // # bits/8
        // allocate space for bytes
        bmapPtr->data = calloc(n_bits, sizeof(uint8_t));
        end = bytes;
        } else {    // if there are EXTRA bytes
        bytes = ((n_bits / 8) + 1); // include extra byte
        bmapPtr->data = calloc(n_bits, sizeof(uint8_t));
        end = bytes + 1;
        }
        */

        bmapPtr->data = calloc(n_bits, sizeof(uint8_t));
        bmapPtr->bit_count = n_bits;
        bmapPtr->byte_count = ceil(n_bits/8);   // ceiling for any remaining bytes

        return bmapPtr;     // return bitmap pointer
    }
    return NULL;
}

bool bitmap_set(bitmap_t *const bitmap, const size_t bit)
{
    if(bitmap && bit < SIZE_MAX - 1 && bit <= bitmap->bit_count) {
        size_t i = bit / 8;       // find the byte
        size_t pos = bit % 8;     // find position
        uint8_t flag = 1;           // flag = 0000 ... 0001
        flag = flag << pos;         // shifted bit positions
        bitmap->data[i] = bitmap->data[i] | flag;   // bitap data OR flag
        return true;
    }
    return false;
}

bool bitmap_reset(bitmap_t *const bitmap, const size_t bit)
{
    if(bitmap && bit >= 0 && bit <= bitmap->bit_count) {
        size_t i = bit / 8;
        size_t pos = bit % 8;
        uint8_t flag = 1;
        flag = flag << pos;
        flag = ~flag;   // flip binary bits to binary one's comp
        // reset the bit
        bitmap->data[i] = bitmap->data[i] & flag;
        return true;
    }
    return false;
}

bool bitmap_test(const bitmap_t *const bitmap, const size_t bit)
{
    if(bitmap && bit >= 0 && bit <= bitmap->bit_count) {
        int i = bit / 8;
        int pos = bit % 8;
        uint8_t flag = 1; 
        flag = flag << pos; 
        if (bitmap->data[i] & flag )  
            return true;
    }
    return false;
}

size_t bitmap_ffs(const bitmap_t *const bitmap)
{
    if(bitmap) {
        size_t i;
        // recursive loop through bitmap to check each bit
        for(i = 0; i < bitmap->bit_count; i++) {
            if(bitmap_test(bitmap, i))
                return i;
        }
    }
    return SIZE_MAX;
}

size_t bitmap_ffz(const bitmap_t *const bitmap)
{
    if(bitmap) {
        size_t i;
        for(i = 0; i < bitmap->bit_count; i++) {
            if(!bitmap_test(bitmap, i))
                return i;
        }
    }
    return SIZE_MAX;
}

bool bitmap_destroy(bitmap_t *bitmap)
{
    if(bitmap && bitmap->data) {
        free(bitmap->data);         // free space
        bitmap->data = NULL;
        free(bitmap);
        bitmap = NULL;

        if(!bitmap)
            return true;
    }
    return false;
}
