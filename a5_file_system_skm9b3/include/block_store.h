#ifndef BLOCK_STORAGE_H__
#define BLOCK_STORAGE_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <bitmap.h>

    // Declaring the struct but not implementing in the header allows us to prevent users
    //  from using the object directly and monkeying with the contents
    // They can only create pointers to the struct, which must be given out by us
    // This enforces a black box device, but it can be restricting
    typedef struct block_store block_store_t;

    ///
    /// This creates a new BS device, ready to go
    /// \return Pointer to a new block storage device, NULL on error
    ///
    //block_store_t *block_store_create();

    ///
    ///// Creates a new back_store file at the specified location
    /////  and returns a back_store object linked to it
    ///// \param fname the file to create
    ///// \return a pointer to the new object, NULL on error
    /////
    block_store_t *block_store_create(const char *const fname);

    ///
    ///// Opens the specified back_store file
    /////  and returns a back_store object linked to it
    ///// \param fname the file to open
    ///// \return a pointer to the new object, NULL on error
    /////
    block_store_t *block_store_open(const char *const fname);

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
 
    

    //////////////////////////////////////////////////////////////////////
    /// some added library functions for this specific implementation  ///
    //////////////////////////////////////////////////////////////////////

    // model the inode table as a blockstore and create a blockstore_t object for it.
    block_store_t *block_store_inode_create(void *const BM_start_pos, void *const data_start_pos);

    // model the file descriptor table as a blockstore and create a block_t object for it.
    block_store_t *block_store_fd_create();

    // return a pointer to the Data of a storage device, NULL on error
    uint8_t * block_store_Data_location(block_store_t *const bs);

    // destroy the blockstore for inode table
    void block_store_inode_destroy(block_store_t *const bs);

    // destroy the blockstore for the file descriptor table
    void block_store_fd_destroy(block_store_t *const bs);

    // search for a free block, marks it as in use, and returns the block's id.
    // Used for the blockstores of the inode table and file descriptor table.
    size_t block_store_sub_allocate(block_store_t *const bs);

    // test if a certain block is available.
    // Used for the blockstores of the inode table and file descriptor table.
    bool block_store_sub_test(block_store_t *const bs, const size_t block_id);

    // release usage of a certain block.
    // Used for the blockstores of the inode table and file descriptor table.
    void block_store_sub_release(block_store_t *const bs, const size_t block_id);

    // read out the inode object in block_id to buffer
    size_t block_store_inode_read(const block_store_t *const bs, const size_t block_id, void *buffer);

    // read out the file descriptor object in block_id to buffer
    size_t block_store_fd_read(const block_store_t *const bs, const size_t block_id, void *buffer);

    // write the inode object in block_id from buffer
    size_t block_store_inode_write(block_store_t *const bs, const size_t block_id, const void *buffer);

    // write the file descriptor object in block_id from buffer
    size_t block_store_fd_write(block_store_t *const bs, const size_t block_id, const void *buffer);

    // reads from data buffer and writes n bytes into block
    size_t block_store_n_write(block_store_t *const bs, const size_t block_id, size_t offset, const void *buffer, size_t bytes);

    /// block store test if in use
    bool block_store_test(block_store_t *const bs, const size_t block_id);

    bitmap_t* block_store_get_bm(block_store_t* const bs);

#ifdef __cplusplus
}
#endif


#endif
