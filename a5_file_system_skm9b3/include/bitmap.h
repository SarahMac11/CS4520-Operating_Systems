#ifndef BITMAP_H__
#define BITMAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct bitmap bitmap_t;

// WARNING: Bit requests outside the bitmap and NULL pointers WILL result in a segfault
// This was originally a high performance C++ library, so the C translation assumes you're using it right.

// But is there really such a thing as a high-performance shared library?

///
/// Sets requested bit in bitmap
/// \param bitmap The bitmap
/// \param bit The bit to set
///
void bitmap_set(bitmap_t *const bitmap, const size_t bit);

///
/// Clears requested bit in bitmap
/// \param bitmap The bitmap
/// \param bit The bit to clear
///
void bitmap_reset(bitmap_t *const bitmap, const size_t bit);

///
/// Returns bit in bitmap
/// \param bitmap The bitmap
/// \param bit The bit to query
/// \return State of requested bit
///
bool bitmap_test(const bitmap_t *const bitmap, const size_t bit);

///
/// Flips bit in bitmap
/// \param bitmap The bitmap
/// \param bit The bit to flip
///
void bitmap_flip(bitmap_t *const bitmap, const size_t bit);

///
/// Flips all bits in the bitmap
/// \param bitmap The bitmap to invert
///
void bitmap_invert(bitmap_t *const bitmap);

///
/// Find first set
/// \param bitmap The bitmap
/// \return The first one bit address, SIZE_MAX on error/not found
///
size_t bitmap_ffs(const bitmap_t *const bitmap);

///
/// Find first zero
/// \param bitmap The bitmap
/// \return The first zero bit address, SIZE_MAX on error/not found
///
size_t bitmap_ffz(const bitmap_t *const bitmap);

///
/// Count all bits set
/// \param bitmap the bitmap
/// \return the total number of bits that are set in the bitmap
///
size_t bitmap_total_set(const bitmap_t *const bitmap);
///
/// For each loop for all set bits
///  (Arguments passed to func are saved across calls)
/// \param bitmap The bitmap
/// \param func The function to apply (first parameter will be size_t with the bit number)
/// \param args A generic pointer to pass to the called function
///
void bitmap_for_each(const bitmap_t *const bitmap, void (*func)(size_t, void *), void *arg);

///
/// Resets bitmap contents to the desired pattern
/// (pattern not guarenteed accurate for final bits
/// 		if bit_count is not a multiple of 8)
/// \param bitmap The bitmap
/// \param pattern The pattern to apply to all bytes
///
void bitmap_format(bitmap_t *const bitmap, const uint8_t pattern);

///
/// Gets total number of bits in bitmap
/// \param bitmap The bitmap
/// \return The number of bits in the bitmap
///
size_t bitmap_get_bits(const bitmap_t *const bitmap);

///
/// Gets total number of bytes in bitmap
/// \param bitmap The bitmap
/// \return number of bytes used by bitmap storage array
///
size_t bitmap_get_bytes(const bitmap_t *const bitmap);

///
/// Creates a bitmap to contain n bits (zero initialized)
/// \param n_bits
/// \return New bitmap pointer, NULL on error
///
bitmap_t *bitmap_create(const size_t n_bits);

///
/// Gets pointer to the internal data for exporting
///  Be sure to query the bit and byte size if it's unknown
/// \param bitmap The bitmap
/// \return Pointer for writing
///
const uint8_t *bitmap_export(const bitmap_t *const bitmap);

///
/// Creates a new bitmap with the provided data
/// Note: This does not use the buffer but copies the data
///  to an internal buffer
/// \param n_bits The number of bits in the bitmap
/// \param bitmap_data The data to import
/// \return New bitmap pointer, NULL on error
///
bitmap_t *bitmap_import(const size_t n_bits, const void *const bitmap_data);

///
/// Creates a new bitmap using the provided data
/// Note: This uses the given block of memory
///  and does not free this pointer on destruction
/// \param n_bits The number of bits in the bitmap
/// \param bitmap_data The data to import
/// \return New bitmap pointer, NULL on error
///
bitmap_t *bitmap_overlay(const size_t n_bits, void *const bitmap_data);

///
/// Destructs and destroys bitmap object
/// \param bitmap The bitmap
///
void bitmap_destroy(bitmap_t *bitmap);


#ifdef __cplusplus
}
#endif

#endif
