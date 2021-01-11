#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "block_store.h"
#include "bitmap.h"

#define BLOCK_STORE_NUM_BLOCKS 65536    // 2^16 blocks.
#define BLOCK_STORE_AVAIL_BLOCKS 65534  // Last 2 blocks consumed by the FBM
#define BLOCK_SIZE_BITS 32768           // 2^12 BYTES per block *2^3 BITS per BYTES
#define BLOCK_SIZE_BYTES 4096           // 2^12 BYTES per block
#define BLOCK_STORE_NUM_BYTES (BLOCK_STORE_NUM_BLOCKS * BLOCK_SIZE_BYTES)  // 2^16 blocks of 2^12 bytes.

#define number_inodes 256
#define inode_size 64

#define number_fd 256
#define fd_size 6   // any number as you see fit

// Block Store Struct
struct block_store {
    int fd;
    uint8_t *data_blocks;
    bitmap_t *fbm;
};

int create_file(const char *const fname) {
    if (fname) {
        int fd = open(fname, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd != -1) {
            if (ftruncate(fd, BLOCK_STORE_NUM_BYTES) != -1) {
                return fd;
            }
            close(fd);
        }
    }
    return -1;
}

int check_file(const char *const fname) {
    if (fname) {
        int fd = open(fname, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd != -1) {
            struct stat file_info;
            if (fstat(fd, &file_info) != -1 && file_info.st_size >= BLOCK_STORE_NUM_BYTES && file_info.st_size <= BLOCK_STORE_NUM_BYTES + BLOCK_STORE_NUM_BYTES/8 ) {
                //if (fstat(fd, &file_info) != -1 && file_info.st_size == BLOCK_STORE_NUM_BYTES) {
                return fd;
            }
            close(fd);
            }
        }
        return -1;
    }

    block_store_t *block_store_init(const bool init, const char *const fname) {
        if (fname) {
            block_store_t *bs = (block_store_t *) malloc(sizeof(block_store_t));
            if (bs) {
                bs->fd = init ? create_file(fname) : check_file(fname);
                if (bs->fd != -1) {
                    bs->data_blocks = (uint8_t *) mmap(NULL, BLOCK_STORE_NUM_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, bs->fd, 0);
                    if (bs->data_blocks != (uint8_t *) MAP_FAILED) {
                        if (init) {
                            memset(bs->data_blocks, 0X00, BLOCK_STORE_NUM_BYTES);
                            bs->data_blocks[BLOCK_STORE_NUM_BYTES - 1] = 0xff;
                            // in case you are trying to write to the bitmap, that will be a disaster
                        }
                        bs->fbm = bitmap_overlay(BLOCK_STORE_AVAIL_BLOCKS, bs->data_blocks + BLOCK_STORE_AVAIL_BLOCKS*BLOCK_SIZE_BYTES);
                        if (bs->fbm) {
                            return bs;
                        }
                        munmap(bs->data_blocks, BLOCK_STORE_NUM_BYTES);
                    }
                    close(bs->fd);
                }
                free(bs);
            }
        }
        return NULL;
    }

    ///
    ///-- Create a new BS device.
    ///-- Return pointer to the new block storage device, NULL on error
    ///
    block_store_t *block_store_create(const char *const fname) {
        return block_store_init(true, fname);
    }

    //
    block_store_t *block_store_open(const char *const fname) {
        return block_store_init(false, fname);
    }

    ///
    ///-- Destroy the provided block storage device
    ///-- \param bs BS device
    ///
    void block_store_destroy(block_store_t *const bs) {
        if (bs) {
            bitmap_destroy(bs->fbm);
            munmap(bs->data_blocks, BLOCK_STORE_NUM_BYTES);
            close(bs->fd);
            free(bs);
        }
    }

    ///
    ///-- Search for a free block, marks it as in use, and return the block's id
    /// \param bs BS device
    /// \return Allocated block's id, SIZE_MAX on error
    ///
    size_t block_store_allocate(block_store_t *const bs) {
        if (bs == NULL) {
            return SIZE_MAX; // return SIZE_MAX if the input is a null pointer
        }
        //-- find first zero in the bitmap
        size_t id;
        id = bitmap_ffz(bs->fbm); // index of the first free block
        if (id == SIZE_MAX) {
            return SIZE_MAX; // return SIZE_MAX since the last block is not available for storing data
        }
        bitmap_set(bs->fbm, id); // mark it as in use
        //  bitmap_destroy(bs->fbm); // destruct and destroy bitmap object
        return id;
    }

    ///
    ///-- Attempts to allocate the requested block id
    /// \param bs the block store object
    /// \block_id the requested block identifier
    /// \return boolean indicating succes of operation
    ///
    bool block_store_request(block_store_t *const bs, const size_t block_id) {
        if (block_id > BLOCK_STORE_AVAIL_BLOCKS || bs == NULL) {
            return false;
        }
        bool blockUsed = 0;
        blockUsed = bitmap_test(bs->fbm, block_id); // check if the block is in use
        if (blockUsed) { // if this block is already in use
            //bitmap_destroy(bs->fbm); // destruct and destroy bitmap object
            return false;
        }
        else { // if this block is not in use
            bitmap_set(bs->fbm, block_id); // mark the block as in use
            //bitmap_destroy(bs->fbm); // destruct and destroy bitmap object
            return true;
        }
    }

    ///
    ///-- Frees the specified block
    /// \param bs BS device
    /// \param block_id The block to free
    ///
    void block_store_release(block_store_t *const bs, const size_t block_id) {
        if (block_id <= BLOCK_STORE_AVAIL_BLOCKS && bs != NULL) {
            bool success = 0;
            success = bitmap_test(bs->fbm, block_id); // check if the block is in use
            if (success) {
                bitmap_reset(bs->fbm, block_id); // clear requested bit in bitmap
                //        bitmap_destroy(bs->fbm); // destruct and destroy bitmap object
            }
        }
        //// Some error message here ////
    }

    ///
    ///-- Counts the number of blocks marked as in use
    /// \param bs BS device
    /// \return Total blocks in use, SIZE_MAX on error
    ///
    size_t block_store_get_used_blocks(const block_store_t *const bs) {
        if (bs) {
            size_t numSet = 0;
            numSet = bitmap_total_set(bs->fbm); // count all bits set
            //  bitmap_destroy(bs->fbm); // destruct and destroy bitmap object
            return numSet;
        }
        return SIZE_MAX;
    }

    ///
    ///-- Counts the number of blocks marked free for use
    /// \param bs BS device
    /// \return Total blocks free, SIZE_MAX on error
    ///
    size_t block_store_get_free_blocks(const block_store_t *const bs) {
        if (bs) {
            size_t numSet = 0;
            size_t numZero = 0;
            numSet = bitmap_total_set(bs->fbm); // count all bits set
            //bitmap_destroy(bs->fbm); // destruct and destroy bitmap object
            numZero = BLOCK_STORE_AVAIL_BLOCKS - numSet; // count zero bits
            return numZero;
        }
        return SIZE_MAX;
    }

    ///
    ///-- Returns the total number of user-addressable blocks
    ///  (since this is constant, you don't even need the bs object)
    /// \return Total blocks
    ///
    size_t block_store_get_total_blocks() {
        return BLOCK_STORE_AVAIL_BLOCKS ;
    }

    ///
    ///-- Reads data from the specified block and writes it to the designated buffer
    /// \param bs BS device
    /// \param block_id Source block id
    /// \param buffer Data buffer to write to
    /// \return Number of bytes read, 0 on error
    ///
    size_t block_store_read(const block_store_t *const bs, const size_t block_id, void *buffer) {
        if (bs && buffer && block_id <= BLOCK_STORE_AVAIL_BLOCKS) {
            memcpy(buffer, bs->data_blocks+block_id*BLOCK_SIZE_BYTES, BLOCK_SIZE_BYTES);
            return BLOCK_SIZE_BYTES;
        }
        return 0;
    }


    ///
    ///-- Reads data from the specified buffer and writes it to the designated block
    /// \param bs BS device
    /// \param block_id Destination block id
    /// \param buffer Data buffer to read from
    /// \return Number of bytes written, 0 on error
    ///
    size_t block_store_write(block_store_t *const bs, const size_t block_id, const void *buffer) {
        if (bs && buffer && block_id <= BLOCK_STORE_AVAIL_BLOCKS) {
            memcpy(bs->data_blocks+block_id*BLOCK_SIZE_BYTES, buffer, BLOCK_SIZE_BYTES);
            return BLOCK_SIZE_BYTES;
        }
        return 0;
    }


    ///
    ///-- Imports BS device from the given file - for grads/bonus
    /// \param filename The file to load
    /// \return Pointer to new BS device, NULL on error
    ///
    block_store_t *block_store_deserialize(const char *const filename) {
        if (filename) {
            int fd = open(filename, O_RDONLY); // open file (read only)
            if (fd < 0) { // if opening file fails
                return 0;
            }
            block_store_t *bs = NULL;
            bs = block_store_create(filename);
            int df_read1, df_read2;
            df_read1 = read(fd, bs->data_blocks, BLOCK_STORE_AVAIL_BLOCKS*BLOCK_SIZE_BYTES); // read bs->Data from the file
            df_read2 = read(fd, bs->fbm, BLOCK_STORE_NUM_BLOCKS/8); // read bs->FBM from the file
            if (df_read1 < 0 || df_read2 < 0) { // if the system call returns an error
                return 0;
            }
            return bs;
        }
        return 0;
    }

    ///
    ///-- Writes the entirety of the BS device to file, overwriting it if it exists - for grads/bonus
    /// \param bs BS device
    /// \param filename The file to write to
    /// \return Number of bytes written, 0 on error
    ///
    size_t block_store_serialize(const block_store_t *const bs, const char *const filename) {
        if (bs && filename) {
            int fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR); // open file (write only)
            if (fd < 0) { // if opening file fails
                return 0;
            }
            write(fd, bs->data_blocks, BLOCK_STORE_AVAIL_BLOCKS*BLOCK_SIZE_BYTES); // write bs->Data to file
            write(fd, bs->fbm, BLOCK_STORE_NUM_BLOCKS/8); // write bs->FBM to file
            close(fd); // close file
            size_t wr_size = block_store_get_used_blocks(bs); // number of block in use
            return (wr_size*BLOCK_SIZE_BYTES); // return number of bytes written
        }
        return 0;
    }


    ///// NEW LIB FUNKS GOTTA HAVE THAT FUNK /////

    ///
    ///-- Creates a new inode BS
    /// \param bm_start bitmap starting position
    /// \param data_start data starting position
    /// \return bs new inode BS, NULL on error
    ///
    block_store_t *block_store_inode_create(void *const BM_start_pos, void *const data_start_pos)
    {
        block_store_t* BS = (block_store_t*)malloc(sizeof(block_store_t));
        if(BS != NULL)  // pointer of the new block store has successfully created
        {
            BS->fbm = bitmap_overlay(256, BM_start_pos);
            BS->data_blocks = data_start_pos;       
            return BS;
        }
        return NULL;
    }


    ///
    ///-- Create file descriptor table of storage device
    /// \return bs file descriptor BS, NULL on error
    ///
    block_store_t *block_store_fd_create()
    {
        block_store_t* BS = (block_store_t*)malloc(sizeof(block_store_t));
        if(BS != NULL)  // pointer of the new block store has successfully created
        {
            BS->data_blocks = calloc(256, 6);   // create space for the blocks
            BS->fbm = bitmap_create(256);
            return BS;
        }
        return NULL;
    }


    ///
    /// -- This returns pointer to start of the Data of a block store
    /// \param bs BS device
    /// \return pointer to start of data in storage device, NULL on error
    ///
    uint8_t * block_store_Data_location(block_store_t *const bs)
    {
        if(bs != NULL)
        {
            return bs->data_blocks;
        }
        return NULL;
    }


    ///
    /// -- Destroy provided inode block storage
    /// \param bs BS inode device
    ///
    void block_store_inode_destroy(block_store_t *const bs)
    {
        if (bs)
        {
            bitmap_destroy(bs->fbm);        // since fbm and data_blocks are in the same memory space, we cannot free the space twice!
            free(bs);
        }
    }


    ///
    /// -- Destroy provided file descriptor storage
    /// \param bs BS file desc device
    ///
    void block_store_fd_destroy(block_store_t *const bs)
    {
        if (bs)
        {
            bitmap_destroy(bs->fbm);        // since fbm and data_blocks are in the same memory space, we cannot free the space twice!
            free(bs->data_blocks);
            free(bs);
        }
    }

    /// -- Create subdirectory
    /// \param bs BS device
    /// \return block store id, SIZE_MAX on error
    ///
    size_t block_store_sub_allocate(block_store_t *const bs) {
        if (bs == NULL) {
            return SIZE_MAX; // return SIZE_MAX if the input is a null pointer
        }
        //-- find first zero in the bitmap
        size_t id;
        id = bitmap_ffz(bs->fbm); // index of the first free block
        if (id == SIZE_MAX) {
            return SIZE_MAX; // return SIZE_MAX since the last block is not available for storing data
        }
        bitmap_set(bs->fbm, id); // mark it as in use
        return id;
    }

    ///
    /// -- Block store subdirectory test if in use
    /// \param bs BS device
    /// \param block_id source block id
    /// \return true if test passed, false on error
    ///
    bool block_store_sub_test(block_store_t *const bs, const size_t block_id) {
        if (block_id > 255 || bs == NULL) {
            return false;
        }
        bool blockUsed = 0;
        blockUsed = bitmap_test(bs->fbm, block_id); // check if the block is in use
        if (blockUsed) { // if this block is already in use
            return true;
        }
        return false;
    }

    ///
    /// -- Block store test if in use
    /// \param bs BS device
    /// \param block_id source block id
    /// \return true if test passed, false on error
    ///
    bool block_store_test(block_store_t *const bs, const size_t block_id) {
        if (block_id >= BLOCK_STORE_AVAIL_BLOCKS || bs == NULL) {
            return false;
        }
        bool blockUsed = 0;
        blockUsed = bitmap_test(bs->fbm, block_id); // check if the block is in use
        if (blockUsed) { // if this block is already in use
            return true;
        }
        else { // if this block is not in use
            return false;
        }
    }

    ///
    /// -- Release block store subdirectory
    /// \param bs BS device
    /// \param block_id source block id
    ///
    void block_store_sub_release(block_store_t *const bs, const size_t block_id) {
        if (block_id < 256 && bs != NULL) {
            bool success = 0;
            success = bitmap_test(bs->fbm, block_id); // check if the block is in use
            if (success) {
                bitmap_reset(bs->fbm, block_id); // clear requested bit in bitmap
            }
        }
        //// Some error message here ////
    }

    ///
    /// -- Read data from inode object into inode block buffer
    /// \param bs BS device
    /// \param block_id source block id
    /// \param buffer data buffer to write to
    /// \return inode size read, 0 on error
    ///
    size_t block_store_inode_read(const block_store_t *const bs, const size_t block_id, void *buffer) {
        if (bs && buffer && block_id <= 255) {
            memcpy(buffer, bs->data_blocks+block_id * 64, 64);
            return 64;
        }
        return 0;
    }

    ///
    /// -- Read data from fd object into designated fd buffer
    /// \param bs BS device
    /// \param block_id srouce block_id
    /// \param buffer data buffer to write to
    /// \return fd size read, 0 on error
    ///
    size_t block_store_fd_read(const block_store_t *const bs, const size_t block_id, void *buffer) {
        if (bs && buffer && block_id <= 255) {
            memcpy(buffer, bs->data_blocks+block_id * 6, 6);
            return 6;
        }
        return 0;
    }


    ///
    /// -- Reads data from buffer and writes into designated inode block
    /// \param bs BS device
    /// \param block_id destination block id
    /// \param buffer data buffer to read from
    /// \return inode size written, 0 on error
    ///
    size_t block_store_inode_write(block_store_t *const bs, const size_t block_id, const void *buffer) {
        if (bs && buffer && block_id < 256) {
            memcpy(bs->data_blocks+block_id*64, buffer, 64);
            return 64;
        }
        return 0;
    } 

    ///
    /// -- Reads data from buffer and writes into designated fd block
    /// \param bs BS device
    /// \param block_id destination block id
    /// \param buffer data buffer to read from
    /// \return fd size written, 0 on error
    ///
    size_t block_store_fd_write(block_store_t *const bs, const size_t block_id, const void *buffer) {
        if (bs && buffer && block_id < 256) {
            memcpy(bs->data_blocks+block_id*6, buffer, 6);
            return 6;
        }
        return 0;
    }

    ///
    /// -- Reads from data buffer and writes n bytes into block
    /// \param bs BS device
    /// \param block_id destination block id to be written to
    /// \param offset
    /// \param buffer data buffer to be read from
    /// \param bytes n bytes to be written
    /// \return bytes written, 0 on error
    ///
    size_t block_store_n_write(block_store_t *const bs, const size_t block_id, size_t offset, const void *buffer, size_t bytes) {
        // error check parameters
        if (bs && buffer && block_id <= BLOCK_STORE_AVAIL_BLOCKS && offset < 512 && (offset + bytes) <= BLOCK_SIZE_BYTES) {
            memcpy(bs->data_blocks+block_id*BLOCK_SIZE_BYTES+offset, buffer, bytes);
            return bytes;
        }
        return 0;
    }

    bitmap_t *block_store_get_bm(block_store_t* const bs) {
        if (bs) {
            return bs->fbm;
        }
        return NULL;
    }
