#ifndef ALLOCATION_H
#define ALLOCATION_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

// Calloc
// - Initializes memory to 0 (Slower than malloc on some operating systems).
// - Checks for integer overflow as the size of the space is calculated internally.
// - Program touches memory immediately guaranteeing it is avaiable.
// - May utilize copy-on-write to initialize: https://blogs.fau.de/hager/archives/825
// Malloc
// - Does not initialize memory (making it slightly faster on some operating systems).
// - Does not check for integer overflow (Making it less likely to fail in comparison to malloc)
// - Program does not touch memory during malloc and the memory is not guaranteed to be there see:
//	"By default, Linux follows an optimistic memory allocation strategy. This means that when malloc() returns non-NULL there is no guarantee that the memory really is available.
//	in case it turns out that the system is out of memory, one or more processes will be killed by the OOM killer." for more information typ: "man malloc" without quotes in terminal.
// - Can be followed by memset in order to initialize memory.
// These differences among amothers make the behavior between calloc and malloc somewhat unpredictable.
// If a size of zero is given either function may choose to return a unique pointer rather than Null.
// Mistakes with any memory management function can lead to overflow into other parts of the program making the program behave unpredicatably, such issues can often be found with Valgrind.
// Allocation marks the start AND end of a region allowing free to deallocate the memory with just the pointer.

/// Allocates an array of n elements of size member_size in bytes utilizing calloc if the clear flag is set, malloc otherwise.
///	Testing with certain values will show different behavior between calloc and malloc.
/// \param size_t - member size in bytes (can be gotten using the sizeof operator e.g. sizeof(int)
/// \param size_t - number of members in the array
///	\param bool -   1 will clear memory (calling calloc), 0 will not (calling malloc).
/// \return void* - pointer to allocated memory.
void* allocate_array(size_t member_size, size_t nmember,bool clear);

/// Realloc
/// - Does not initialize expanded memory
/// - Original contents remain unchanged
/// - If passed a null pointer then a malloc will occur.
/// - May move memory to a new location

/// Simple wrapper around realloc.
/// \param void* - pointer to memory to resize.
/// \param size_t - size of memory to allocate
/// \return void* - pointer to reallocated memory region, may be same as original pointer.
void* reallocate_array(void* ptr, size_t size);

// Free
// - Should not be called on a null pointer or a pointer not received from allocation.
// - Free does not reinitialize the memory region.
// - An "Invalid Pointer" error may be a sign of bad memory operations or an overflow from a memset, memcpy, or allocation or freeing a pointer twice.
// - If the received pointer is null no operation is performed.

/// Wrapper around free. Frees memory and sets received pointer to NULL.
/// \param void* - pointer to memory to free.
/// \return Nothing
void deallocate_array(void** ptr);

// Heap & Stack
// - Local variables are allocated on the stack
// - Large local variable can overflow the stack as stack space is limited (the stack is shared with the functions your program calls as well)
// - When a stack variable leaves scope it is popped from the stack meaning you cannot return a local variable (stack variable) from a function.
// - Heap variables are allocated in memory (or in other places, e.g. using MMAP)
// - Heap variables can be vastly larger than stack variables
// - A heap variable remains available until it is freed, it is the programmers job to do so.
// - A heap variable that is not freed results in a memory leak, such leaks can be found using valgrind.
// - An overflow of the heap can cause serious issues in other parts of the program that may not be easily found. Such issues can usually be found with valgrind -v
// - Allocation of stack variables is faster as you only need to alter the stack pointer.

/// Takes a file name and reads a line into a newly allocated buffer
/// \param char* - filename to read from
/// \return char* - Pointer to malloced heap space containing buffer
char* read_line_to_buffer(char* filename);

#ifdef __cplusplus
    }
#endif
#endif //ALLOCATION_H
