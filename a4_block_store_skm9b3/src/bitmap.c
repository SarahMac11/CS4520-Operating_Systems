#include "bitmap.h"
#include <string.h>

// Just the one for now. Indicates we're an overlay and should not free
// (also, make sure that ALL is as wide as ll of the flags)
typedef enum { NONE = 0x00, OVERLAY = 0x01, ALL = 0xFF } BITMAP_FLAGS;

struct bitmap 
{
    unsigned leftover_bits;  // Packing will increase this to an int anyway
    BITMAP_FLAGS flags;      // Generic place to store flags. Not enough flags to worry about width yet.
    uint8_t *data;
    size_t bit_count, byte_count;
};

#define FLAG_CHECK(bitmap, flag) ((bitmap)->flags & flag)
// Not sure I want these
// #define FLAG_SET(bitmap, flag) bitmap->flags |= flag
// #define FLAG_UNSET(bitmap, flag) bitmap->flags &= ~flag

// lookup instead of always shifting bits. Should be faster? Confirmed: 10% faster
// Also, using native int width because it should be faster as well? - Negligible/indeterminate
//  Won't help until bitmap uses native width for the array
static const uint8_t mask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

// Mask for all bits at index i and lower
static const uint8_t mask_down_inclusive[8] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};

// Inverted mask
static const uint8_t invert_mask[8] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};

// Way more testing than I should waste my time on suggested uint8_t was faster
// but it may still be negligible/indeterminate.
// Since the data store is uint8_t, we already get punished for our bad alignment
// so this doesn't really matter until everything gets moved to generic int

// Total bits set in the given byte in a handy lookup table
// Macros, man...
// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetTable
#define B2(n) n, n + 1, n + 1, n + 2
#define B4(n) B2(n), B2(n + 1), B2(n + 1), B2(n + 2)
#define B6(n) B4(n), B4(n + 1), B4(n + 1), B4(n + 2)
static const uint8_t bit_totals[256] = {B6(0), B6(1), B6(1), B6(2)};
#undef B6
#undef B4
#undef B2
// There is an alternative for getting bit count that only loops as many times as there are bits set
// but that's still a loop and this table is 256B.
/*
   unsigned int v; // count the number of bits set in v
   unsigned int c; // c accumulates the total bits set in v

   for (c = 0; v; v >>= 1)
   {
   c += v & 1;
   }
 */

// A place to generalize the creation process and setup
bitmap_t *bitmap_initialize(size_t n_bits, BITMAP_FLAGS flags);

void bitmap_set(bitmap_t *const bitmap, const size_t bit) 
{
    bitmap->data[bit >> 3] |= mask[bit & 0x07];
}

void bitmap_reset(bitmap_t *const bitmap, const size_t bit) 
{
    bitmap->data[bit >> 3] &= invert_mask[bit & 0x07];
}

bool bitmap_test(const bitmap_t *const bitmap, const size_t bit) 
{
    return bitmap->data[bit >> 3] & mask[bit & 0x07];
}

void bitmap_flip(bitmap_t *const bitmap, const size_t bit) 
{
    bitmap->data[bit >> 3] ^= mask[bit & 0x07];
}

void bitmap_invert(bitmap_t *const bitmap) 
{
    for (size_t byte = 0; byte < bitmap->byte_count; ++byte) 
    {
        bitmap->data[byte] = ~bitmap->data[byte];
    }
}

size_t bitmap_ffs(const bitmap_t *const bitmap) 
{
    if (bitmap) 
    {
        size_t result = 0;
        for (; result < bitmap->bit_count && !bitmap_test(bitmap, result); ++result) 
        {
        }
        return (result == bitmap->bit_count ? SIZE_MAX : result);
    }
    return SIZE_MAX;
}

size_t bitmap_ffz(const bitmap_t *const bitmap) 
{
    if (bitmap) 
    {
        size_t result = 0;
        for (; result < bitmap->bit_count && bitmap_test(bitmap, result); ++result) 
        {
        }
        return (result == bitmap->bit_count ? SIZE_MAX : result);
    }
    return SIZE_MAX;
}

size_t bitmap_total_set(const bitmap_t *const bitmap) 
{
    size_t total = 0;
    if (bitmap) 
    {
        // If we have leftover, stop a byte early because we have to handle it differently.
        size_t stop = bitmap->leftover_bits ? bitmap->byte_count - 1 : bitmap->byte_count;
        for (size_t idx = 0; idx < stop; ++idx) 
        {
            total += bit_totals[bitmap->data[idx]];
        }
        if (bitmap->leftover_bits) 
        {
            // haha, this is readable
            // get the byte at the end of the bitmap, mask it so we're only looking at the bits in use
            // then feed that to the bit_total lookup so we don't count the bits past our bit total
            // (which whould be considered undetermined)
            total += bit_totals[bitmap->data[bitmap->byte_count - 1] & mask_down_inclusive[bitmap->leftover_bits - 1]];
        }
    }
    return total;
}

void bitmap_for_each(const bitmap_t *const bitmap, void (*func)(size_t, void *), void *arg) 
{
    if (bitmap && func) 
    {
        for (size_t idx = 0; idx < bitmap->bit_count; ++idx) 
        {
            if (bitmap_test(bitmap, idx)) 
            {
                func(idx, arg);
            }
        }
    }
}

void bitmap_format(bitmap_t *const bitmap, const uint8_t pattern) 
{
    memset(bitmap->data, pattern, bitmap->byte_count);
}

size_t bitmap_get_bits(const bitmap_t *const bitmap) 
{
    return bitmap->bit_count;
}

size_t bitmap_get_bytes(const bitmap_t *const bitmap) 
{
    return bitmap->byte_count;
}

bitmap_t *bitmap_create(const size_t n_bits) 
{
    return bitmap_initialize(n_bits, NONE);
}

const uint8_t *bitmap_export(const bitmap_t *const bitmap) 
{
    return bitmap->data;
}

bitmap_t *bitmap_import(const size_t n_bits, const void *const bitmap_data) 
{
    if (bitmap_data) 
    {
        bitmap_t *bitmap = bitmap_initialize(n_bits, NONE);
        if (bitmap) 
        {
            memcpy(bitmap->data, bitmap_data, bitmap->byte_count);
            return bitmap;
        }
    }
    return NULL;
}

bitmap_t *bitmap_overlay(const size_t n_bits, void *const bitmap_data) 
{
    if (bitmap_data) 
    {
        bitmap_t *bitmap = bitmap_initialize(n_bits, OVERLAY);
        if (bitmap) 
        {
            bitmap->data = (uint8_t *) bitmap_data;
            return bitmap;
        }
    }
    return NULL;
}

void bitmap_destroy(bitmap_t *bitmap) 
{
    if (bitmap) 
    {
        if (!FLAG_CHECK(bitmap, OVERLAY)) 
        {
            // don't free memory that isn't ours!
            free(bitmap->data);
        }
        free(bitmap);
    }
}

//
///
// HERE BE DRAGONS
///
//

bitmap_t *bitmap_initialize(size_t n_bits, BITMAP_FLAGS flags) 
{
    if (n_bits) 
    {  // must be non-zero
        bitmap_t *bitmap = (bitmap_t *) malloc(sizeof(bitmap_t));
        if (bitmap) 
        {
            bitmap->flags         = flags;
            bitmap->bit_count     = n_bits;
            bitmap->byte_count    = n_bits >> 3;
            bitmap->leftover_bits = n_bits & 0x07;
            bitmap->byte_count += (bitmap->leftover_bits ? 1 : 0);

            // FLAG HANDLING HERE

            // This logic will need to be reworked when we have more than one flag, haha
            // Maybe something like if (flags) and then contain a giant if/else-if for each flag
            // Then a return at the end

            if (FLAG_CHECK(bitmap, OVERLAY)) 
            {
                // don't mess with data, caller will set it
                bitmap->data = NULL;
                return bitmap;
            } 
            else 
            {
                bitmap->data = (uint8_t *) calloc(bitmap->byte_count, 1);
                if (bitmap->data) 
                {
                    return bitmap;
                }
            }

            free(bitmap);
        }
    }
    return NULL;
}
