#ifndef BLOCK_STORAGE_H__
#define BLOCK_STORAGE_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include <stdlib.h>
#include <stdbool.h>

	// Constants
#define BLOCK_STORE_NUM_BLOCKS 512  
// available blocks - first block taken by FBM
#define BLOCK_STORE_AVAIL_BLOCKS (BLOCK_STORE_NUM_BLOCKS - 1)
#define BLOCK_SIZE_BYTES 32         // 2^5 BYTES per block
#define BITMAP_SIZE_BITS BLOCK_STORE_NUM_BLOCKS
#define BITMAP_SIZE_BYTES (BITMAP_SIZE_BITS / 8)
#define BLOCK_STORE_NUM_BYTES (BLOCK_STORE_NUM_BLOCKS * BLOCK_SIZE_BYTES)
#define BITMAP_START_BLOCK 127
#define BITMAP_NUM_BLOCKS (BITMAP_SIZE_BYTES/BLOCK_SIZE_BYTES)

	// Declaring the struct but not implementing in the header allows us to prevent users
	//  from using the object directly and monkeying with the contents
	// They can only create pointers to the struct, which must be given out by us
	// This enforces a black box device, but it can be restricting
	typedef struct block_store block_store_t;

	///
	/// This creates a new BS device, ready to go
	/// \return Pointer to a new block storage device, NULL on error
	///
	block_store_t *block_store_create();

	///
	/// Destroys the provided block storage device
	/// This is an idempotent operation, so there is no return value
	/// \param bs BS device
	///
	void block_store_destroy(block_store_t *const bs);

	///
	/// Searches for a free block, marks it as in use, and returns the block's id
	/// \param bs BS device
	/// \return Allocated block's id, SIZE_MAX on error
	///
	size_t block_store_allocate(block_store_t *const bs);

	///
	/// Attempts to allocate the requested block id
	/// \param bs the block store object
	/// \block_id the requested block identifier
	/// \return boolean indicating succes of operation
	///
	bool block_store_request(block_store_t *const bs, const size_t block_id);

	///
	/// Frees the specified block
	/// \param bs BS device
	/// \param block_id The block to free
	///
	void block_store_release(block_store_t *const bs, const size_t block_id);

	///
	/// Counts the number of blocks marked as in use
	/// \param bs BS device
	/// \return Total blocks in use, SIZE_MAX on error
	///
	size_t block_store_get_used_blocks(const block_store_t *const bs);

	///
	/// Counts the number of blocks marked free for use
	/// \param bs BS device
	/// \return Total blocks free, SIZE_MAX on error
	///
	size_t block_store_get_free_blocks(const block_store_t *const bs);

	///
	/// Returns the total number of user-addressable blocks
	///  (since this is constant, you don't even need the bs object)
	/// \return Total blocks
	///
	size_t block_store_get_total_blocks();

	///
	/// Reads data from the specified block and writes it to the designated buffer
	/// \param bs BS device
	/// \param block_id Source block id
	/// \param buffer Data buffer to write to
	/// \return Number of bytes read, 0 on error
	///
	size_t block_store_read(const block_store_t *const bs, const size_t block_id, void *buffer);

	///
	/// Reads data from the specified buffer and writes it to the designated block
	/// \param bs BS device
	/// \param block_id Destination block id
	/// \param buffer Data buffer to read from
	/// \return Number of bytes written, 0 on error
	///
	size_t block_store_write(block_store_t *const bs, const size_t block_id, const void *buffer);

	///
	/// Imports BS device from the given file - for grads/bonus
	/// \param filename The file to load
	/// \return Pointer to new BS device, NULL on error
	///
	block_store_t *block_store_deserialize(const char *const filename);

	///
	/// Writes the entirety of the BS device to file, overwriting it if it exists - for grads/bonus
	/// \param bs BS device
	/// \param filename The file to write to
	/// \return Number of bytes written, 0 on error
	///
	size_t block_store_serialize(const block_store_t *const bs, const char *const filename);

#ifdef __cplusplus
}
#endif


#endif
