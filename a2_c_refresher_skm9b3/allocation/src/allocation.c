#include "../include/allocation.h"
#include <stdlib.h>
#include <stdio.h>


void* allocate_array(size_t member_size, size_t nmember,bool clear)
{
    // error check params
    if(member_size <= 0 || nmember <= 0) return NULL;
    void *ptr = NULL;
    // check if malloc or calloc
    if(clear)
        ptr = (void *)calloc(nmember, member_size);
    else
        ptr = (void *)malloc(nmember * member_size);
    return ptr;
}

void* reallocate_array(void* ptr, size_t size)
{
    // error check params
    if(!ptr || size <= 0) return NULL;

    ptr = (void *)realloc(ptr, size);
    return ptr;
}

void deallocate_array(void** ptr)
{
    // error check param
    if(*ptr == NULL)
        return;
    free(*ptr);
    *ptr = NULL;
    return;
}

char* read_line_to_buffer(char* filename)
{
    if(!filename)
        return NULL;
    FILE *fp = fopen(filename, "r");
    // allocate with buffer BUFSIZ = constant macro representing the size of the input buffer
    char *ptr = malloc(BUFSIZ * (sizeof(char)));
    fgets(ptr, BUFSIZ, fp);
    fclose(fp);
    return ptr;
}
