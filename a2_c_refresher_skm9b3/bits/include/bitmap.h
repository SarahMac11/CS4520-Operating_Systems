#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// useful link:
// http://www.mathcs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html

typedef struct bitmap {
    uint8_t *data;
    size_t bit_count, byte_count;
} bitmap_t;

/// Creates a bitmap to contain 'n' bits (zero initialized)
/// \param n_bits
/// \return New bitmap pointer, NULL on error
///
bitmap_t *bitmap_create(const size_t n_bits);

///
/// Sets the requested bit in bitmap
/// \param bitmap The bitmap
/// \param bit The bit to set
///
bool bitmap_set(bitmap_t *const bitmap, const size_t bit);

///
/// Clears requested bit in bitmap
/// \param bitmap The bitmap
/// \param bit The bit to be cleared
///
bool bitmap_reset(bitmap_t* const bitmap, const size_t bit);

///
/// Returns bit in bitmap
/// \param bitmap The bitmap
/// \param bit The bit to queried
/// \return State of requested bit
///
bool bitmap_test(const bitmap_t *const bitmap, const size_t bit);

/// Find the first set bit
/// \param bitmap The bitmap
/// \return The first one bit address, SIZE_MAX on error/Not found
///
size_t bitmap_ffs(const bitmap_t *const bitmap);

/// Find first zero bit
/// \param bitmap The bitmap
/// \return The first zero bit address, SIZE_MAX on error/Not found
///
size_t bitmap_ffz(const bitmap_t *const bitmap);

/// Destructs and destroys bitmap object
/// \param bit The bitmap
/// \return The Success or Failure of destruct and destroy bitmap object
///
bool bitmap_destroy(bitmap_t *bitmap);

#endif

