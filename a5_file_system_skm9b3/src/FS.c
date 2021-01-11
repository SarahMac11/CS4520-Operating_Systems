#include "dyn_array.h"
#include "bitmap.h"
#include "block_store.h"
#include "FS.h"

// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

#define NUM_DIRECT_PTR 6
#define NUM_INDIRECT_PTR 512
#define NUM_DOUBLE_DIRECT_PTR 512

#define number_inodes 256
#define inode_size 64
#define number_fd 256
#define fd_size 6    // any number as you see fit

#define folder_number_entries 31

// Inode Struct
struct inode 
{
    uint32_t vacantFile;    // this parameter is only for directory. Used as a bitmap denoting availibility of entries in a directory file.
    char owner[18];         // for alignment purpose only 
    char fileType;          // 'r' denotes regular file, 'd' denotes directory file

    size_t inodeNumber;         // for FS, the range should be 0-255
    size_t fileSize;              // the unit is in byte    
    size_t linkCount;

    // to realize the 16-bit addressing, pointers are acutally block numbers, rather than 'real' pointers.
    uint16_t directPointer[6];
    uint16_t indirectPointer[1];
    uint16_t doubleIndirectPointer;
};

// File Descriptor Struct
struct fileDescriptor 
{
    uint8_t inodeNum;   // the inode # of the fd

    // usage, locate_order and locate_offset together locate the exact byte at which the cursor is 
    uint8_t usage;       // inode pointer usage info. Only the lower 3 digits will be used. 1 for direct, 2 for indirect, 4 for dbindirect
    uint16_t locate_order;       // serial number or index of the block within direct, indirect, or dbindirect range
    uint16_t locate_offset;      // offset of the cursor within a block
};

struct directoryFile {
    char filename[127];
    uint8_t inodeNumber;
};

// File System Sruct
struct FS {
    block_store_t * BlockStore_whole;
    block_store_t * BlockStore_inode;
    block_store_t * BlockStore_fd;
};


///// ADDITIONAL HELPER FUNCTIONS /////
///
/// Checks if the input filename is valid or not
/// \param filename is filename to be validated
/// \return true if valid, else false
///
bool isValidFileName(const char *filename)
{
    if(!filename || strlen(filename) == 0 || strlen(filename) > 31)     // some "big" number as you wish
    {
        return false;
    }

    // define invalid characters might be contained in filenames
    char *invalidCharacters = "!@#$%^&*?\"";
    int i = 0;
    int len = strlen(invalidCharacters);
    for( ; i < len; i++)
    {
        if(strchr(filename, invalidCharacters[i]) != NULL)
        {
            return false;
        }
    }
    return true;
}

///
/// Used to decompose the input string into filenames along the path, '/'
/// \param a_str input filename path
/// \param a_delim delimitor split in filename path
/// \param count elements extracted from path
/// \return result of path with tokens
///
char** str_split(char* a_str, const char a_delim, size_t * count)
{
    if(*a_str != '/')
    {
        return NULL;
    }
    char** result    = 0;
    char* tmp        = a_str;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = '\0';
    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            (*count)++;
        }
        tmp++;
    }

    result = (char**)calloc(1, sizeof(char*) * (*count));
    for(size_t i = 0; i < (*count); i++)
    {
        *(result + i) = (char*)calloc(1, 200);
    }

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            strcpy(*(result + idx++), token);
            token = strtok(NULL, delim);
        }

    }
    return result;
}


///
/// Formats (and mounts) an FS file for use
/// \param fname The file to format
/// \return Mounted FS object, NULL on error
///
FS_t *fs_format(const char *path)
{
    if(path != NULL && strlen(path) != 0)
    {

        FS_t * ptr_FS = (FS_t *)calloc(1, sizeof(FS_t));	// get started
        ptr_FS->BlockStore_whole = block_store_create(path);				// pointer to start of a large chunck of memory

        // reserve the 1st block for bitmap of inode
        size_t bitmap_ID = block_store_allocate(ptr_FS->BlockStore_whole);
        //		printf("bitmap_ID = %zu\n", bitmap_ID);

        // 2rd - 5th block for inodes, 4 blocks in total
        size_t inode_start_block = block_store_allocate(ptr_FS->BlockStore_whole);
        //		printf("inode_start_block = %zu\n", inode_start_block);		
        for(int i = 0; i < 3; i++)
        {
            block_store_allocate(ptr_FS->BlockStore_whole);
            //			printf("all the way with block %zu\n", block_store_allocate(ptr_FS->BlockStore_whole));
        }

        // install inode block store inside the whole block store
        ptr_FS->BlockStore_inode = block_store_inode_create(block_store_Data_location(ptr_FS->BlockStore_whole) + bitmap_ID * BLOCK_SIZE_BYTES, block_store_Data_location(ptr_FS->BlockStore_whole) + inode_start_block * BLOCK_SIZE_BYTES);

        // the first inode is reserved for root dir
        block_store_sub_allocate(ptr_FS->BlockStore_inode); 
        //		printf("first inode ID = %zu\n", block_store_sub_allocate(ptr_FS->BlockStore_inode));

        // update the root inode info.
        uint8_t root_inode_ID = 0;	// root inode is the first one in the inode table
        inode_t * root_inode = (inode_t *) calloc(1, sizeof(inode_t));
        //		printf("size of inode_t = %zu\n", sizeof(inode_t));
        root_inode->vacantFile = 0x00000000;
        root_inode->fileType = 'd';								
        root_inode->inodeNumber = root_inode_ID;
        root_inode->linkCount = 1;
        //		root_inode->directPointer[0] = root_data_ID;	// not allocate date block for it until it has a sub-folder or file
        block_store_inode_write(ptr_FS->BlockStore_inode, root_inode_ID, root_inode);		

        free(root_inode);

        // now allocate space for the file descriptors
        ptr_FS->BlockStore_fd = block_store_fd_create();

        return ptr_FS;
    }

    return NULL;	
}



///
/// Mounts an FS object and prepares it for use
/// \param fname The file to mount
/// \return Mounted FS object, NULL on error
///
FS_t *fs_mount(const char *path)
{
    if(path != NULL && strlen(path) != 0)
    {

        FS_t * ptr_FS = (FS_t *)calloc(1, sizeof(FS_t));	// get started
        ptr_FS->BlockStore_whole = block_store_open(path);	// get the chunck of data	 

        // the bitmap block should be the 1st one
        size_t bitmap_ID = 0;

        // the inode blocks start with the 2nd block, and goes around until the 5th block, 4 in total
        size_t inode_start_block = 1;

        // attach the bitmaps to their designated place
        ptr_FS->BlockStore_inode = block_store_inode_create(block_store_Data_location(ptr_FS->BlockStore_whole) + bitmap_ID * BLOCK_SIZE_BYTES, block_store_Data_location(ptr_FS->BlockStore_whole) + inode_start_block * BLOCK_SIZE_BYTES);

        // since file descriptors are allocated outside of the whole blocks, we can simply reallocate space for it.
        ptr_FS->BlockStore_fd = block_store_fd_create();

        return ptr_FS;
    }

    return NULL;		
}


///
/// Unmounts the given object and frees all related resources
/// \param fs The FS object to unmount
/// \return 0 on success, < 0 on failure
///
int fs_unmount(FS_t *fs)
{
    if(fs != NULL)
    {	
        block_store_inode_destroy(fs->BlockStore_inode);

        block_store_destroy(fs->BlockStore_whole);
        block_store_fd_destroy(fs->BlockStore_fd);

        free(fs);
        return 0;
    }
    return -1;
} 
///
/// Creates a new file at the specified location
///   Directories along the path that do not exist are not created
/// \param fs The FS containing the file
/// \param path Absolute path to file to create
/// \param type Type of file to create (regular/directory)
/// \return 0 on success, < 0 on failure
///
int fs_create(FS_t *fs, const char *path, file_t type)
{
    if(fs != NULL && path != NULL && strlen(path) != 0 && (type == FS_REGULAR || type == FS_DIRECTORY))
    {
        char* copy_path = (char*)calloc(1, 65535);
        strcpy(copy_path, path); 
        char** tokens;		// tokens are the directory names along the path. The last one is the name for the new file or dir
        size_t count = 0;
        tokens = str_split(copy_path, '/', &count);
        free(copy_path);
        if(tokens == NULL)
        {
            return -1;
        }

        // let's check if the filenames are valid or not
        for(size_t n = 0; n < count; n++)
        {	
            if(isValidFileName(*(tokens + n)) == false)
            {
                // before any return, we need to free tokens, otherwise memory leakage
                for (size_t i = 0; i < count; i++)
                {
                    free(*(tokens + i));
                }
                free(tokens);
                return -1;
            }
        }

        size_t parent_inode_ID = 0;	// start from the 1st inode, ie., the inode for root directory
        // first, let's find the parent dir
        size_t indicator = 0;

        // we declare parent_inode and parent_data here since it will still be used after the for loop
        directoryFile_t * parent_data = (directoryFile_t *)calloc(1, BLOCK_SIZE_BYTES);

        inode_t * parent_inode = (inode_t *) calloc(1, sizeof(inode_t));	

        for(size_t i = 0; i < count - 1; i++)
        {
            block_store_inode_read(fs->BlockStore_inode, parent_inode_ID, parent_inode);	// read out the parent inode
            // in case file and dir has the same name
            if(parent_inode->fileType == 'd')
            {
                block_store_read(fs->BlockStore_whole, parent_inode->directPointer[0], parent_data);

                for(int j = 0; j < folder_number_entries; j++)
                {
                    if( ((parent_inode->vacantFile >> j) & 1) == 1 && strcmp((parent_data + j) -> filename, *(tokens + i)) == 0 )
                    {
                        parent_inode_ID = (parent_data + j) -> inodeNumber;
                        indicator++;
                    }					
                }
            }					
        }
        //		printf("indicator = %zu\n", indicator);
        //		printf("parent_inode_ID = %lu\n", parent_inode_ID);

        // read out the parent inode
        block_store_inode_read(fs->BlockStore_inode, parent_inode_ID, parent_inode);
        if(indicator == count - 1 && parent_inode->fileType == 'd')
        {
            // same file or dir name in the same path is intolerable
            for(int m = 0; m < folder_number_entries; m++)
            {
                // rid out the case of existing same file or dir name
                if( ((parent_inode->vacantFile >> m) & 1) == 1)
                {
                    // before read out parent_data, we need to make sure it does exist!
                    block_store_read(fs->BlockStore_whole, parent_inode->directPointer[0], parent_data);
                    if( strcmp((parent_data + m) -> filename, *(tokens + count - 1)) == 0 )
                    {
                        free(parent_data);
                        free(parent_inode);	
                        // before any return, we need to free tokens, otherwise memory leakage
                        for (size_t i = 0; i < count; i++)
                        {
                            free(*(tokens + i));
                        }
                        free(tokens);
                        return -1;											
                    }
                }
            }	


            // cannot declare k inside for loop, since it will be used later.
            int k = 0;
            for( ; k < folder_number_entries; k++)
            {
                if( ((parent_inode->vacantFile >> k) & 1) == 0 )
                    break;
            }

            // if k == 0, then we have to declare a new parent data block
            //			printf("k = %d\n", k);
            if(k == 0)
            {
                size_t parent_data_ID = block_store_allocate(fs->BlockStore_whole);
                //					printf("parent_data_ID = %zu\n", parent_data_ID);
                if(parent_data_ID < BLOCK_STORE_AVAIL_BLOCKS)
                {
                    parent_inode->directPointer[0] = parent_data_ID;
                }
                else
                {
                    free(parent_inode);
                    free(parent_data);
                    // before any return, we need to free tokens, otherwise memory leakage
                    for (size_t i = 0; i < count; i++)
                    {
                        free(*(tokens + i));
                    }
                    free(tokens);
                    return -1;												
                }
            }

            if(k < folder_number_entries)	// k == folder_number_entries means this directory is full
            {
                size_t child_inode_ID = block_store_sub_allocate(fs->BlockStore_inode);
                // printf("new child_inode_ID = %zu\n", child_inode_ID);
                // ugh, inodes are used up
                if(child_inode_ID == SIZE_MAX)
                {
                    free(parent_data);
                    free(parent_inode);
                    // before any return, we need to free tokens, otherwise memory leakage
                    for (size_t i = 0; i < count; i++)
                    {
                        free(*(tokens + i));
                    }
                    free(tokens);
                    return -1;	
                }

                // wow, at last, we make it!				
                // update the parent inode
                parent_inode->vacantFile |= (1 << k);
                // in the following cases, we should allocate parent data first: 
                // 1)the parent dir is not the root dir; 
                // 2)the file or dir to create is to be the 1st in the parent dir

                block_store_inode_write(fs->BlockStore_inode, parent_inode_ID, parent_inode);	

                // update the parent directory file block
                block_store_read(fs->BlockStore_whole, parent_inode->directPointer[0], parent_data);
                strcpy((parent_data + k)->filename, *(tokens + count - 1));
                //				printf("the newly created file's name is: %s\n", (parent_data + k)->filename);

                (parent_data + k)->inodeNumber = child_inode_ID;
                block_store_write(fs->BlockStore_whole, parent_inode->directPointer[0], parent_data);

                // update the newly created inode
                inode_t * child_inode = (inode_t *) calloc(1, sizeof(inode_t));
                child_inode->vacantFile = 0;
                if(type == FS_REGULAR)
                {
                    child_inode->fileType = 'r';
                }
                else if(type == FS_DIRECTORY)
                {
                    child_inode->fileType = 'd';
                }	

                child_inode->inodeNumber = child_inode_ID;
                child_inode->fileSize = 0;
                child_inode->linkCount = 1;
                block_store_inode_write(fs->BlockStore_inode, child_inode_ID, child_inode);

                //				printf("after creation, parent_inode->vacantFile = %d\n", parent_inode->vacantFile);

                // free the temp space
                free(parent_inode);
                free(parent_data);
                free(child_inode);
                // before any return, we need to free tokens, otherwise memory leakage
                for (size_t i = 0; i < count; i++)
                {
                    free(*(tokens + i));
                }
                free(tokens);					
                return 0;
            }				
        }
        // before any return, we need to free tokens, otherwise memory leakage
        for (size_t i = 0; i < count; i++)
        {
            free(*(tokens + i));
        }
        free(tokens);
        free(parent_inode);	
        free(parent_data);
    }
    return -1;
}


///
/// Opens the specified file for use
///   R/W position is set to the beginning of the file (BOF)
///   Directories cannot be opened
/// \param fs The FS containing the file
/// \param path path to the requested file
/// \return file descriptor to the requested file, < 0 on error
///
int fs_open(FS_t *fs, const char *path)
{
    if(fs != NULL && path != NULL && strlen(path) != 0)
    {
        char* copy_path = (char*)calloc(1, 65535);
        strcpy(copy_path, path);
        char** tokens;		// tokens are the directory names along the path. The last one is the name for the new file or dir
        size_t count = 0;
        tokens = str_split(copy_path, '/', &count);
        free(copy_path);
        if(tokens == NULL)
        {
            return -1;
        }

        // let's check if the filenames are valid or not
        for(size_t n = 0; n < count; n++)
        {	
            if(isValidFileName(*(tokens + n)) == false)
            {
                // before any return, we need to free tokens, otherwise memory leakage
                for (size_t i = 0; i < count; i++)
                {
                    free(*(tokens + i));
                }
                free(tokens);
                return -1;
            }
        }	

        size_t parent_inode_ID = 0;	// start from the 1st inode, ie., the inode for root directory
        // first, let's find the parent dir
        size_t indicator = 0;

        inode_t * parent_inode = (inode_t *) calloc(1, sizeof(inode_t));
        directoryFile_t * parent_data = (directoryFile_t *)calloc(1, BLOCK_SIZE_BYTES);			

        // locate the file
        for(size_t i = 0; i < count; i++)
        {		
            block_store_inode_read(fs->BlockStore_inode, parent_inode_ID, parent_inode);	// read out the parent inode
            if(parent_inode->fileType == 'd')
            {
                block_store_read(fs->BlockStore_whole, parent_inode->directPointer[0], parent_data);
                //printf("parent_inode->vacantFile = %d\n", parent_inode->vacantFile);
                for(int j = 0; j < folder_number_entries; j++)
                {
                    //printf("(parent_data + j) -> filename = %s\n", (parent_data + j) -> filename);
                    if( ((parent_inode->vacantFile >> j) & 1) == 1 && strcmp((parent_data + j) -> filename, *(tokens + i)) == 0 )
                    {
                        parent_inode_ID = (parent_data + j) -> inodeNumber;
                        indicator++;
                    }					
                }
            }					
        }		
        free(parent_data);			
        free(parent_inode);	
        //printf("indicator = %zu\n", indicator);
        //printf("count = %zu\n", count);
        // now let's open the file
        if(indicator == count)
        {
            size_t fd_ID = block_store_sub_allocate(fs->BlockStore_fd);
            //printf("fd_ID = %zu\n", fd_ID);
            // it could be possible that fd runs out
            if(fd_ID < number_fd)
            {
                size_t file_inode_ID = parent_inode_ID;
                inode_t * file_inode = (inode_t *) calloc(1, sizeof(inode_t));
                block_store_inode_read(fs->BlockStore_inode, file_inode_ID, file_inode);	// read out the file inode	

                // it's too bad if file to be opened is a dir 
                if(file_inode->fileType == 'd')
                {
                    free(file_inode);
                    // before any return, we need to free tokens, otherwise memory leakage
                    for (size_t i = 0; i < count; i++)
                    {
                        free(*(tokens + i));
                    }
                    free(tokens);
                    return -1;
                }

                // assign a file descriptor ID to the open behavior
                fileDescriptor_t * fd = (fileDescriptor_t *)calloc(1, sizeof(fileDescriptor_t));
                fd->inodeNum = file_inode_ID;
                fd->usage = 1;
                fd->locate_order = 0; // R/W position is set to the beginning of the file (BOF)
                fd->locate_offset = 0;
                block_store_fd_write(fs->BlockStore_fd, fd_ID, fd);

                free(file_inode);
                free(fd);
                // before any return, we need to free tokens, otherwise memory leakage
                for (size_t i = 0; i < count; i++)
                {
                    free(*(tokens + i));
                }
                free(tokens);			
                return fd_ID;
            }	
        }
        // before any return, we need to free tokens, otherwise memory leakage
        for (size_t i = 0; i < count; i++)
        {
            free(*(tokens + i));
        }
        free(tokens);
    }
    return -1;
}


///
/// Closes the given file descriptor
/// \param fs The FS containing the file
/// \param fd The file to close
/// \return 0 on success, < 0 on failure
///
int fs_close(FS_t *fs, int fd)
{
    if(fs != NULL && fd >=0 && fd < number_fd)
    {
        // first, make sure this fd is in use
        if(block_store_sub_test(fs->BlockStore_fd, fd))
        {
            block_store_sub_release(fs->BlockStore_fd, fd);
            return 0;
        }   
    }
    return -1;
}

///
/// Populates a dyn_array with information about the files in a directory
///   Array contains up to 15 file_record_t structures
/// \param fs The FS containing the file
/// \param path Absolute path to the directory to inspect
/// \return dyn_array of file records, NULL on error
///
dyn_array_t *fs_get_dir(FS_t *fs, const char *path)
{
    if(fs != NULL && path != NULL && strlen(path) != 0)
    {   
        char* copy_path = (char*)malloc(200);
        strcpy(copy_path, path);
        char** tokens;      // tokens are the directory names along the path. The last one is the name for the new file or dir
        size_t count = 0;
        tokens = str_split(copy_path, '/', &count);
        free(copy_path);

        if(strlen(*tokens) == 0)
        {
            // a spcial case: only a slash, no dir names
            count -= 1;
        }
        else
        {
            for(size_t n = 0; n < count; n++)
            {   
                if(isValidFileName(*(tokens + n)) == false)
                {
                    // before any return, we need to free tokens, otherwise memory leakage
                    for (size_t i = 0; i < count; i++)
                    {
                        free(*(tokens + i));
                    }
                    free(tokens);       
                    return NULL;

                }
            }           
        }
        // search along the path and find the deepest dir
        size_t parent_inode_ID = 0; // start from the 1st inode, ie., the inode for root directory
        // first, let's find the parent dir
        size_t indicator = 0;

        inode_t * parent_inode = (inode_t *) calloc(1, sizeof(inode_t));
        directoryFile_t * parent_data = (directoryFile_t *)calloc(1, BLOCK_SIZE_BYTES);
        for(size_t i = 0; i < count; i++)
        {
            block_store_inode_read(fs->BlockStore_inode, parent_inode_ID, parent_inode);    // read out the parent inode
            // in case file and dir has the same name. But from the test cases we can see, this case would not happen
            if(parent_inode->fileType == 'd')
            {           
                block_store_read(fs->BlockStore_whole, parent_inode->directPointer[0], parent_data);
                for(int j = 0; j < folder_number_entries; j++)
                {
                    if( ((parent_inode->vacantFile >> j) & 1) == 1 && strcmp((parent_data + j) -> filename, *(tokens + i)) == 0 )
                    {
                        parent_inode_ID = (parent_data + j) -> inodeNumber;
                        indicator++;
                    }                   
                }   
            }                   
        }   
        free(parent_data);
        free(parent_inode);

        // now let's enumerate the files/dir in it
        if(indicator == count)
        {
            inode_t * dir_inode = (inode_t *) calloc(1, sizeof(inode_t));
            block_store_inode_read(fs->BlockStore_inode, parent_inode_ID, dir_inode);   // read out the file inode          
            if(dir_inode->fileType == 'd')
            {
                // prepare the data to be read out
                directoryFile_t * dir_data = (directoryFile_t *)calloc(1, BLOCK_SIZE_BYTES);
                block_store_read(fs->BlockStore_whole, dir_inode->directPointer[0], dir_data);
                // prepare the dyn_array to hold the data
                dyn_array_t * dynArray = dyn_array_create(15, sizeof(file_record_t), NULL);

                for(int j = 0; j < folder_number_entries; j++)
                {
                    if( ((dir_inode->vacantFile >> j) & 1) == 1 )
                    {
                        file_record_t* fileRec = (file_record_t *)calloc(1, sizeof(file_record_t));
                        strcpy(fileRec->name, (dir_data + j) -> filename);
                        // to know fileType of the member in this dir, we have to refer to its inode
                        inode_t * member_inode = (inode_t *) calloc(1, sizeof(inode_t));
                        block_store_inode_read(fs->BlockStore_inode, (dir_data + j) -> inodeNumber, member_inode);
                        if(member_inode->fileType == 'd')
                        {
                            fileRec->type = FS_DIRECTORY;
                        }
                        else if(member_inode->fileType == 'f')
                        {
                            fileRec->type = FS_REGULAR;
                        }

                        // now insert the file record into the dyn_array
                        dyn_array_push_front(dynArray, fileRec);
                        free(fileRec);
                        free(member_inode);
                    }                  
                }
                free(dir_data);
                free(dir_inode);
                // before any return, we need to free tokens, otherwise memory leakage
                if(strlen(*tokens) == 0)
                {
                    // a spcial case: only a slash, no dir names
                    count += 1;
                }
                for (size_t i = 0; i < count; i++)
                {
                    free(*(tokens + i));
                }
                free(tokens);   
                return(dynArray);
            }
            free(dir_inode);
        }

        // before any return, we need to free tokens, otherwise memory leakage
        if(strlen(*tokens) == 0)
        {
            // a spcial case: only a slash, no dir names
            count += 1;
        }
        for (size_t i = 0; i < count; i++)
        {
            free(*(tokens + i));
        }
        free(tokens);   
    }
    return NULL;

}

off_t fs_seek(FS_t *fs, int fd, off_t offset, seek_t whence)
{
    UNUSED(fs);
    UNUSED(fd);
    UNUSED(offset);
    UNUSED(whence);
    return 0;
}

///
/// Reads data from the file linked to the given descriptor
///   Reading past EOF returns data up to EOF
///   R/W position in incremented by the number of bytes read
/// \param fs The FS containing the file
/// \param fd The file to read from
/// \param dst The buffer to write to
/// \param nbyte The number of bytes to read
/// \return number of bytes read (< nbyte IFF read passes EOF), < 0 on error
///
ssize_t fs_read(FS_t *fs, int fd, void *dst, size_t nbyte)
{
    // error check parameters
    if(!fs || fd < 0 ||  fd >= number_fd || !dst) {
        //printf("fail parameter check\n");
        return -1;
    }

    // bitmap test using block store bitmap
    if(!bitmap_test(block_store_get_bm(fs->BlockStore_fd), fd)) {
        //printf("fsil bitmap test\n");
        return -2;
    }

    // error check nbytes
    if(nbyte == 0) {
        //printf("nbytes 0\n");
        return 0;
    }
    //printf("current nbyte %zu\n", nbyte);

    // define file descriptor
    fileDescriptor_t file_desc;
    if(block_store_fd_read(fs->BlockStore_fd, fd, &file_desc) == 0) {
        //printf("fail fd read\n");
        return 0;
    }
    uint16_t fd_loc = file_desc.locate_order;
    uint16_t fd_off = file_desc.locate_offset;


    // define inode
    uint8_t fd_inode_id = file_desc.inodeNum;
    inode_t fd_inode;
    if(block_store_inode_read(fs->BlockStore_inode, fd_inode_id, &fd_inode) == 0) {
        //printf("failed inode read \n");
        return 0;
    }

    // get previous offset
    off_t head = getPrevOffset(&file_desc);
    //if(head == 333)
    //printf("head 333\n");
    if(head + nbyte > fd_inode.fileSize) {
        nbyte = fd_inode.fileSize - head;
    }

    // total bytes read
    ssize_t total_bytes_read = 0;
    if(fd_loc < NUM_DIRECT_PTR) {
        //printf("fd location < num direct pointer\n");
        total_bytes_read = read_direct_block(fs, &fd_inode, fd_loc, fd_off, dst, nbyte);
    } else if(fd_loc >= NUM_DIRECT_PTR && fd_loc < NUM_INDIRECT_PTR + NUM_DIRECT_PTR) {
        //printf("read indirect\n");
        total_bytes_read = read_indirect_block(fs, &fd_inode, fd_loc, fd_off, dst, nbyte, fd_inode.indirectPointer[0]);
    } else {
        //printf("read double direct block\n");
        total_bytes_read = read_doubleDirect_block(fs, &fd_inode, fd_loc, fd_off, dst, nbyte);
    }

    //if(total_bytes_read == 0)
    //printf("no bytes read\n");

    updateFD(&file_desc, total_bytes_read);
    block_store_fd_write(fs->BlockStore_fd, fd, &file_desc);

    //printf("total bytes read %zu\n", total_bytes_read);
    return total_bytes_read;

}

///
/// Writes data from given buffer to the file linked to the descriptor
///   Writing past EOF extends the file
///   Writing inside a file overwrites existing data
///   R/W position in incremented by the number of bytes written
/// \param fs The FS containing the file
/// \param fd The file to write to
/// \param dst The buffer to read from
/// \param nbyte The number of bytes to write
/// \return number of bytes written (< nbyte IFF out of space), < 0 on error
///
ssize_t fs_write(FS_t *fs, int fd, const void *src, size_t nbyte)
{    
    // error check parameters
    if (!fs || fd < 0 || fd >= number_fd || !src) {
        //printf("failed parameter check\n");
        return -1;
    }
    // error check bitmap test using block store bitmap
    if(!bitmap_test(block_store_get_bm(fs->BlockStore_fd), fd)) { 
        //printf("failt bitmap test\n");
        return -2; 
    }

    // define file descriptor
    fileDescriptor_t file_desc;
    block_store_fd_read(fs->BlockStore_fd, fd, &file_desc);
    uint16_t fd_loc = file_desc.locate_order;
    uint16_t fd_off = file_desc.locate_offset;

    // define inode
    inode_t inode;
    block_store_inode_read(fs->BlockStore_inode, file_desc.inodeNum, &inode);

    ssize_t total_bytes_written = 0;

    if (fd_loc < NUM_DIRECT_PTR) {
        total_bytes_written = write_direct_block(fs, &inode, fd_loc, fd_off, src, nbyte);
    } else if (fd_loc >= NUM_DIRECT_PTR && fd_loc < NUM_INDIRECT_PTR + NUM_DIRECT_PTR) {
        size_t indir_block_id;
        if (inode.indirectPointer[0] == 0) {
            indir_block_id = allocate_indirectPtr_block(fs);
            if (indir_block_id == SIZE_MAX) {
                return 0;
            }
            inode.indirectPointer[0] = indir_block_id;
        } else {
            indir_block_id = inode.indirectPointer[0];
        }
        total_bytes_written = write_indirect_block(fs, &inode, fd_loc, fd_off, src, nbyte, indir_block_id);
    } else if (fd_loc >= NUM_INDIRECT_PTR + NUM_DIRECT_PTR){
        total_bytes_written = write_double_direct_block(fs, &inode, fd_loc, fd_off, src, nbyte);
    }

    // update file descriptor
    updateFD(&file_desc, total_bytes_written);
    block_store_fd_write(fs->BlockStore_fd, fd, &file_desc);

    // update inode
    inode.fileSize += total_bytes_written;
    block_store_inode_write(fs->BlockStore_inode, file_desc.inodeNum, &inode);

    return total_bytes_written;
}

int fs_remove(FS_t *fs, const char *path)
{
    UNUSED(fs);
    UNUSED(path);
    return 0;
}

int fs_move(FS_t *fs, const char *src, const char *dst)
{
    UNUSED(fs);
    UNUSED(src);
    UNUSED(dst);
    return 0;
}

int fs_link(FS_t *fs, const char *src, const char *dst)
{
    UNUSED(fs);
    UNUSED(src);
    UNUSED(dst);
    return 0;
}


///// More Functions /////

///
/// Read direct pointer block
/// \param fs File system
/// \param inode source to be read from
/// \param fd_loc file diretory locator
/// \param fd_off file directory offset
/// \param dst destination to be written
/// \param nbyte bytes read
/// \return bytes read, else 0
///
ssize_t read_direct_block(FS_t *fs, inode_t *inode, uint16_t fd_loc,uint16_t fd_off, void *dst, size_t nbyte)
{
    // error check parameters
    uint16_t block_id = inode->directPointer[fd_loc];
    if(block_id == 0)
        return 0;

    size_t blanks = BLOCK_SIZE_BYTES - fd_off;
    if(blanks > nbyte)
        blanks = nbyte;

    uint8_t file_block[BLOCK_SIZE_BYTES];
    // read entire block store from block id into file block
    block_store_read(fs->BlockStore_whole, block_id, file_block);
    // copy to destination
    memcpy(dst, file_block + fd_off, blanks);

    // remove blank spaces from nbyte
    nbyte -= blanks;
    if(nbyte == 0)
        return blanks;

    if(fd_loc + 1 < 6)
        return blanks + read_direct_block(fs, inode, fd_loc + 1, 0, dst + blanks, nbyte);
    else
        return blanks + read_indirect_block(fs, inode, fd_loc + 1, 0, dst + blanks, nbyte, inode->indirectPointer[0]);
}

///
/// Write to direct pointer block
/// \param fs File system
/// \param inode destination to write to
/// \param fd_loc fd locator
/// \param fd_off fd offset
/// \param src source to be written to buffer
/// \param nbytes bytes written
/// \return written, else 0
///
ssize_t write_direct_block(FS_t *fs, inode_t *inode, uint16_t fd_loc, uint16_t fd_off, const void *src, size_t nbyte)
{
    // buffer
    uint8_t curr_block[BLOCK_SIZE_BYTES];
    memset(curr_block, '\0', BLOCK_SIZE_BYTES);

    size_t blanks = BLOCK_SIZE_BYTES - fd_off;
    if(blanks >  nbyte)
        blanks = nbyte;

    // write source src to current block buffer
    size_t block_id;
    if(inode->directPointer[fd_loc] == 0) {
        block_id = block_store_allocate(fs->BlockStore_whole);
        if(block_id == SIZE_MAX) {
            printf("file block full\n");
            return 0;
        }
        inode->directPointer[fd_loc] = block_id;
        memcpy(curr_block, src, BLOCK_SIZE_BYTES);
    } else {
        block_id = inode->directPointer[fd_loc];
        block_store_read(fs->BlockStore_whole, block_id, curr_block);
        memcpy(curr_block + fd_off, src, blanks);
    }

    block_store_write(fs->BlockStore_whole, block_id, curr_block);

    nbyte -= blanks;
    if(nbyte == 0)
        return blanks;
    if(fd_loc + 1 < fd_size) {
        return blanks + write_direct_block(fs, inode, fd_loc + 1, 0, src + blanks, nbyte);
    } else {
        // indirect pointer
        size_t indir_block_id;
        if(inode->indirectPointer[0] == 0) {
            uint16_t indirectPtr_block_buffer[NUM_INDIRECT_PTR];
            size_t ind_block_id = block_store_allocate(fs->BlockStore_whole);
            if(ind_block_id == SIZE_MAX)
                return 0;
            memset(indirectPtr_block_buffer, '\0', BLOCK_SIZE_BYTES);
            block_store_write(fs->BlockStore_whole, ind_block_id, indirectPtr_block_buffer);
            indir_block_id = ind_block_id;
        } else {
            indir_block_id = inode->indirectPointer[0];
        }
        return blanks + write_indirect_block(fs, inode, fd_loc + 1, 0, src + blanks, nbyte, indir_block_id);
    }
}

///
/// Read indirect pointer block
/// \param fs File system
/// \param inode source to be read from
/// \param fd_loc file directory locator
/// \param fd_off file directory locator offset
/// \param dst destination file to write to
/// \param nbyte bytes read
/// \param indirect_block_id block id
/// \return read bytes, else 0
///
ssize_t read_indirect_block(FS_t *fs, inode_t *inode, uint16_t fd_loc, uint16_t fd_off, void *dst, size_t nbyte, uint16_t indirect_block_id)
{
    // error check parameters
    if(indirect_block_id == 0)
        return 0;

    uint16_t indirect_ptr_buff[NUM_INDIRECT_PTR];
    // read indirect block store into indirect ptr buffer
    block_store_read(fs->BlockStore_whole, indirect_block_id, indirect_ptr_buff);
    uint16_t indirect_ptr_id = (fd_loc - 6) % NUM_DOUBLE_DIRECT_PTR;
    ssize_t bytes_read = 0;

    while(indirect_ptr_id < NUM_INDIRECT_PTR && nbyte > 0) {
        size_t block_id = indirect_ptr_buff[indirect_ptr_id];
        if(block_id == 0)
            return bytes_read;

        uint8_t block_buff[BLOCK_SIZE_BYTES];
        block_store_read(fs->BlockStore_whole, block_id, block_buff);
        size_t blanks = BLOCK_SIZE_BYTES - fd_off;

        if(blanks > nbyte)
            blanks = nbyte;
        memcpy(dst, block_buff + fd_off, blanks);

        // increment values
        nbyte -= blanks;
        indirect_ptr_id += 1;
        dst += blanks;
        bytes_read += blanks;
        fd_loc += 1;
        fd_off = 0;
    }

    if(nbyte == 0)
        return bytes_read;

    return bytes_read + read_doubleDirect_block(fs, inode, fd_loc, fd_off, dst, nbyte);
}

///
/// Write to indirect pointer block
/// \param fs File system
/// \param inode destination to write to
/// \param fd_loc fd locator
/// \param fd_off fd offset
/// \param src source to be written to buffer
/// \param nbytes bytes written
/// \param indir_block_id block id of indirect block pointer
/// \return written, else 0
///
ssize_t write_indirect_block(FS_t *fs, inode_t *inode, uint16_t fd_loc, uint16_t fd_off, const void *src, size_t nbyte, size_t indir_block_id)
{
    uint16_t indir_ptr_buff[NUM_INDIRECT_PTR];
    block_store_read(fs->BlockStore_whole, indir_block_id, indir_ptr_buff);
    uint16_t indir_ptr_id = (fd_loc - fd_size) % NUM_DOUBLE_DIRECT_PTR;
    ssize_t bytes_written = 0;

    // write indirect pointer
    while(indir_ptr_id < NUM_INDIRECT_PTR && nbyte > 0) {
        size_t block_id;
        if(indir_ptr_buff[indir_ptr_id] == 0) {
            block_id = block_store_allocate(fs->BlockStore_whole);
            if(block_id == SIZE_MAX) {
                block_store_write(fs->BlockStore_whole, indir_block_id, indir_ptr_buff);
                return bytes_written;
            }
            indir_ptr_buff[indir_ptr_id] = block_id;
        } else {
            block_id = indir_ptr_buff[indir_ptr_id];
        }

        size_t blanks = BLOCK_SIZE_BYTES - fd_off;
        if(blanks > nbyte)
            blanks = nbyte;

        uint8_t write_buff[BLOCK_SIZE_BYTES];
        block_store_write(fs->BlockStore_whole, block_id, write_buff);

        nbyte -= blanks;
        indir_ptr_id += 1;
        src += blanks;
        bytes_written += blanks;
        fd_loc += 1;
        fd_off = 0;
    }

    block_store_write(fs->BlockStore_whole, indir_block_id, indir_ptr_buff);

    if(nbyte == 0) {
        printf("nbyte = 0\n");
        return bytes_written;
    }

    return bytes_written + write_double_direct_block(fs, inode, fd_loc, fd_off, src, nbyte);
}

///
/// Read double direct pointer block
/// \param fs File system
/// \param inode source to be read from
/// \param fd_loc file directory locator
/// \param fd_off file directory offset
/// \param dst destination to write to
/// \param nbyte bytes read
/// \return bytes read, else 0
///
ssize_t read_doubleDirect_block(FS_t *fs, inode_t *inode, uint16_t fd_loc, uint16_t fd_off, void *dst, size_t nbyte)
{
    // error check parameters
    if(inode->doubleIndirectPointer == 0)
        return 0;

    uint16_t doub_dir_ptr_buff[NUM_DOUBLE_DIRECT_PTR];
    block_store_read(fs->BlockStore_whole, inode->doubleIndirectPointer, doub_dir_ptr_buff);

    // check double direct pointer buffer was written to
    int idx = (fd_loc - (6 + NUM_INDIRECT_PTR)) / NUM_DOUBLE_DIRECT_PTR;
    if(doub_dir_ptr_buff[idx] == 0)
        return 0;

    return read_indirect_block(fs, inode, fd_loc, fd_off, dst, nbyte, doub_dir_ptr_buff[idx]);
}

///
/// Write to double direct pointer block
/// \param fs File system
/// \param inode destination to write to
/// \param fd_loc fd locator
/// \param fd_off fd offset
/// \param src source to be written to buffer
/// \param nbytes bytes written
/// \return written, else 0
///
ssize_t write_double_direct_block(FS_t *fs, inode_t *inode, uint16_t fd_loc, uint16_t fd_off, const void *src, size_t nbyte)
{
    size_t doub_dir_block_id;
    if(inode->doubleIndirectPointer == 0) {
        uint16_t indir_ptr_block_buff[NUM_INDIRECT_PTR];
        size_t block_id = block_store_allocate(fs->BlockStore_whole);
        if(block_id == SIZE_MAX) {
            printf("block id == size max\n");
            return 0;
        }
        memset(indir_ptr_block_buff, '\0', BLOCK_SIZE_BYTES);
        block_store_write(fs->BlockStore_whole, block_id, indir_ptr_block_buff);
        doub_dir_block_id = block_id;

        inode->doubleIndirectPointer = doub_dir_block_id;
    } else {
        doub_dir_block_id = inode->doubleIndirectPointer;
    }

    uint16_t doub_dir_ptr_buff[NUM_DOUBLE_DIRECT_PTR];
    block_store_read(fs->BlockStore_whole, doub_dir_block_id, doub_dir_ptr_buff);
    int idx = (fd_loc - (fd_size + NUM_INDIRECT_PTR)) / NUM_DOUBLE_DIRECT_PTR;

    size_t indir_block_id;
    if(doub_dir_ptr_buff[idx] == 0) {
        uint16_t indir_ptr_block_buff[NUM_INDIRECT_PTR];
        size_t block_id = block_store_allocate(fs->BlockStore_whole);
        if(block_id == SIZE_MAX) {
            printf("block id == size max\n");
            return 0;
        }
        memset(indir_ptr_block_buff, '\0', BLOCK_SIZE_BYTES);
        block_store_write(fs->BlockStore_whole, block_id, indir_ptr_block_buff);
        indir_block_id = block_id;

        doub_dir_ptr_buff[idx] = indir_block_id;
        block_store_write(fs->BlockStore_whole, doub_dir_block_id, doub_dir_ptr_buff);
    } else {
        indir_block_id = doub_dir_ptr_buff[idx];
    }

    return write_indirect_block(fs, inode, fd_loc, fd_off, src, nbyte, indir_block_id);
}

///
/// Update file directory
/// \param fd file descriptor
/// \param nbyte byte count to add to offset
/// 
void updateFD(fileDescriptor_t* fd, ssize_t nbyte) {
    uint16_t blanks = BLOCK_SIZE_BYTES - fd->locate_offset;
    if (blanks >= nbyte) {
        fd->locate_offset += nbyte;
        if (fd->locate_offset == 1024) {
            fd->locate_order += 1;
            fd->locate_offset = 0;
        }
    } else {
        nbyte -= blanks;
        fd->locate_order += 1;
        fd->locate_order += nbyte / BLOCK_SIZE_BYTES;
        fd->locate_offset = nbyte % BLOCK_SIZE_BYTES;
    }
}

///
/// Allocate space for indirect pointer block
/// \param fs File system
/// \return block id allocated for indirect pointer, eles SIZE_MAX on error
///
size_t allocate_indirectPtr_block(FS_t* fs) {
    uint16_t indir_ptr_block_buff[NUM_INDIRECT_PTR];
    size_t block_id = block_store_allocate(fs->BlockStore_whole);
    if (block_id == SIZE_MAX) {
        return SIZE_MAX;
    }
    memset(indir_ptr_block_buff, '\0', BLOCK_SIZE_BYTES);
    block_store_write(fs->BlockStore_whole, block_id, indir_ptr_block_buff);
    return block_id;
}

///
/// Get previous file directory offset
/// \param fd file descriptor to find offset
/// \return offset result
/// 
off_t getPrevOffset(fileDescriptor_t* fileDescriptor) 
{
    // define variables
    off_t res = 0;
    uint16_t loc = fileDescriptor->locate_order;
    uint16_t off = fileDescriptor->locate_offset;
    if (loc == 0) {
        //printf("offset %u\n", off);
        return off;
    }
    //if(loc == 333)
    //printf("location 333\n");
    if (off != 0) {
        res += (loc - 1) * BLOCK_SIZE_BYTES;
        res += off;
    } else {
        res += loc * BLOCK_SIZE_BYTES;
    }
    //printf("result %zu\n", res);
    return res; 
}
