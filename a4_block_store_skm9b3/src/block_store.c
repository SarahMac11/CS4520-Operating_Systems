#include <stdio.h>
#include <stdint.h>
#include "bitmap.h"
#include "block_store.h"
// include more if you need
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
//#define UNUSED(x) (void)(x)

/*
 * Declaring the struct but not implementing in the header allows us to prevent users
 * from using the object directly and monkeying with the contents
 * They can only create pointers to the struct, which must be given out by us
 * This enforces a black box device, but it can be restricting
 */
typedef struct block_store {
    bitmap_t *bitmap;
    // block data includes available blocks and block size in bytes
    char *block_data[BLOCK_STORE_AVAIL_BLOCKS][BLOCK_SIZE_BYTES];
} block_store_t;

/*
 * This creates a new BS device, ready to go
 * \return Pointer to a new block storage device, NULL on error
 */
block_store_t *block_store_create()
{
    // allocate a block of memory for block store
    block_store_t *bs = (block_store_t *)calloc(1, sizeof(block_store_t));

    // check malloc error
    if(!bs) 
        return NULL;

    // bitmap space with available bs blocks
    bs->bitmap = bitmap_create(BLOCK_STORE_AVAIL_BLOCKS);

    if(!bs->bitmap)
        return NULL;

    size_t i;
    for(i = 0; i < BITMAP_SIZE_BYTES; i++) {
        if((i >= BITMAP_START_BLOCK + BITMAP_NUM_BLOCKS) || ((int) i < BITMAP_START_BLOCK))
            // bitmap reset
            bitmap_reset(bs->bitmap, i);
    }
    //printf("\ni block store bs->bitmap reset: %zu\n", i);

    return bs;
}

/*
 * Destroys the provided block storage device
 * This is an idempotent operation, so there is no return value
 * \param bs BS device
 */
void block_store_destroy(block_store_t *const bs)
{
    //UNUSED(bs);
    // error check parameters
    if(!bs || !bs->bitmap) 
        return;      // return no value
    // retrieve bit to destroy
    bitmap_destroy(bs->bitmap);
    free(bs);       // free block store
}

/*
 *Searches for a free block, marks it as in use, and returns the block's id
 * \param bs BS device
 * \return Allocated block's id, SIZE_MAX on error
 */
size_t block_store_allocate(block_store_t *const bs)
{
    //UNUSED(bs);
    // error check parameters
    if(!bs || !bs->block_data)
        return SIZE_MAX;    // return size max on error

    // find first free bit with first zero of bitmap and set to block size
    // size max returned from ffz on error/not found
    size_t ffz = bitmap_ffz(bs->bitmap);

    // error check size
    if(ffz >= SIZE_MAX || ffz > BLOCK_STORE_AVAIL_BLOCKS)
        return SIZE_MAX;    // return size max on error

    // set requested bit in bitmap
    // send bitmap and bit to set
    bitmap_set(bs->bitmap, ffz);

    //printf("\nffz returned: %zu\n", ffz + 1);

    return ffz;
}

/*
 * Attempts to allocate the requested block id
 * \param bs the block store object
 * \block_id the requested block identifier
 * \return boolean indicating succes of operation
 */
bool block_store_request(block_store_t *const bs, const size_t block_id)
{
    //UNUSED(bs);
    //UNUSED(block_id);
    // error check parameters and if bit is already set
    if(!bs || block_id <= 0 || block_id >= BLOCK_STORE_NUM_BLOCKS || bitmap_test(bs->bitmap, block_id))
        return false;     

    // set bit in bitmap
    bitmap_set(bs->bitmap, block_id);

    // return false if bit still not set
    if(bitmap_test(bs->bitmap, block_id) == 0)
        return false;

    return true;
}

/*
 *Frees the specified block
 * \param bs BS device
 * \param block_id The block to free
 */
void block_store_release(block_store_t *const bs, const size_t block_id)
{
    //UNUSED(bs);
    //UNUSED(block_id);
    // error check parameters
    if(!bs || block_id >= BLOCK_STORE_NUM_BLOCKS)
        return;

    // check if bit is already cleared
    if(bitmap_test(bs->bitmap, block_id) == 0)
        return;

    // clear bit
    bitmap_reset(bs->bitmap, block_id);

    return;
}

/*
 * Counts the number of blocks marked as in use
 * \param bs BS device
 * \return Total blocks in use, SIZE_MAX on error
 */
size_t block_store_get_used_blocks(const block_store_t *const bs)
{
    //UNUSED(bs);
    // error check parameters
    if(!bs)
        return SIZE_MAX;    // return size max on error
    // get the total number of bits set in bitmap
    size_t set_blocks = bitmap_total_set(bs->bitmap);
    //printf("\ntotal used blocks: %zu\n", set_blocks);
    return set_blocks;
}

/*
 * Counts the number of blocks marked free for use
 * \param bs BS device
 * \return Total blocks free, SIZE_MAX on error
 */
size_t block_store_get_free_blocks(const block_store_t *const bs)
{
    //UNUSED(bs);
    // error check parameters
    if(!bs)
        return SIZE_MAX;

    // invert bitmap bits
    bitmap_invert(bs->bitmap);
    size_t num_free = 0;
    // get free bits that were flipped
    num_free = bitmap_total_set(bs->bitmap);
    //printf("\nNum free bits inverted: %zu\n", num_free);
    // revert bits
    bitmap_invert(bs->bitmap);
    //printf("\nblock store avail blocks: %d\n", BLOCK_STORE_AVAIL_BLOCKS);
    return num_free;
}

/*
 * Returns the total number of user-addressable blocks
 * (since this is constant, you don't even need the bs object)
 * \return Total blocks
 */
size_t block_store_get_total_blocks()
{
    // return total blocks
    return BLOCK_STORE_NUM_BLOCKS;
}

/*
 * Reads data from the specified block and writes it to the designated buffer
 * \param bs BS device
 * \param block_id Source block id
 * \param buffer Data buffer to write to
 * \return Number of bytes read, 0 on error
 */
size_t block_store_read(const block_store_t *const bs, const size_t block_id, void *buffer)
{
    //UNUSED(bs);
    //UNUSED(block_id);
    //UNUSED(buffer);
    // error check parameters
    if(!bs || block_id > BLOCK_STORE_NUM_BYTES || !buffer)
        return 0;
    // copy bs block data at block id into buffer
    if(!memcpy(buffer, bs->block_data[block_id], BLOCK_SIZE_BYTES))
        return 0;
    // return bytes read
    return BLOCK_SIZE_BYTES;
}

/*
 * Reads data from the specified buffer and writes it to the designated block
 * \param bs BS device
 * \param block_id Destination block id
 * \param buffer Data buffer to read from
 * \return Number of bytes written, 0 on error
 */
size_t block_store_write(block_store_t *const bs, const size_t block_id, const void *buffer)
{
    //UNUSED(bs);
    //UNUSED(block_id);
    //UNUSED(buffer);
    // error check parameters
    if(!bs || block_id >= BLOCK_STORE_NUM_BYTES || !buffer)
        return 0;

    // copy buffer into bs block data at block id
    memcpy(bs->block_data[block_id], buffer, BLOCK_SIZE_BYTES);
    // return bytes written
    return BLOCK_SIZE_BYTES;
}

/*
 * Imports BS device from the given file - for grads/bonus
 * \param filename The file to load
 * \return Pointer to new BS device, NULL on error
 */
block_store_t *block_store_deserialize(const char *const filename)
{
    //UNUSED(filename);
    // error check parameters
    if(!filename)
        return NULL;

    // open file to read
    int fd = open(filename, O_RDONLY);
    // error check file descriptor
    if(fd < 0)
        return NULL;

    block_store_t *bs = (block_store_t *)malloc(sizeof(block_store_t));
    // malloc memory for buffer
    void *buffer = (bitmap_t *)malloc(BITMAP_SIZE_BYTES * sizeof(char));
    // read file with bufer using bitmap size bytes
    int r = read(fd, buffer, BITMAP_SIZE_BYTES);
    // error check read
    if(r < 0)
        return NULL;
    // create new bitmap to store in bs
    bs->bitmap = bitmap_import(BITMAP_SIZE_BYTES, buffer);
    if(!bs->bitmap)
        return NULL;
    free(buffer);

    // malloc memory for block data
    **(bs)->block_data = (char *)malloc(BLOCK_STORE_AVAIL_BLOCKS * BLOCK_STORE_NUM_BLOCKS * sizeof(char));

    // read from file to block of block data
    read(fd, **bs->block_data, BLOCK_STORE_NUM_BLOCKS * BLOCK_STORE_AVAIL_BLOCKS);
    // close file
    int c = close(fd);
    // error chek close
    if(c < 0)
        return NULL;
    // return pointer to new bs device
    return bs;
}

/*
 * Writes the entirety of the BS device to file, overwriting it if it exists - for grads/bonus
 * \param bs BS device
 * \param filename The file to write to
 * \return Number of bytes written, 0 on error
 */
size_t block_store_serialize(const block_store_t *const bs, const char *const filename)
{
    //UNUSED(bs);
    //UNUSED(filename);
    // error check parameters
    if(!bs || !filename)
        return 0;

    // define flags
    int f = O_WRONLY | O_CREAT | O_TRUNC;
    // open file
    int fd = open(filename, f, 0644);
    // error check file descriptor
    if(fd < 0)
        return 0;

    // write to file using block store num bytes
    int w = write(fd, bs, BLOCK_STORE_NUM_BYTES);
    // error check write
    if(w < 0)
        return 0;
    int c = close(fd);
    if(c < 0)
        return 0;

    // return # of bytes written
    return w;
}

