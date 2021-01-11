#include "../include/debug.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

// protected function, that only this .c can use
int comparator_func(const void *a, const void *b) {
    return (*(uint8_t *)a - *(uint8_t *)b);
}

bool terrible_sort(uint16_t *data_array, const size_t value_count) {

    // error check params
    if(value_count < 1 || data_array == NULL) {
        return false; 
    }

    uint16_t *sorting_array = (uint16_t *)malloc(value_count * sizeof(uint16_t));
    if(!sorting_array) {
        return false;
    }
    // loop through to copy items into sorting array
    for (uint16_t i = 0; i < value_count; ++i)
        sorting_array[i] = data_array[i];

    qsort(sorting_array, value_count, sizeof(uint16_t), comparator_func);

    for (uint16_t i = 0; i < value_count; ++i) {
        if (sorting_array[i-1] > sorting_array[i]){
            return false;
        }
    }
    memcpy(data_array, sorting_array, value_count * sizeof(uint16_t));
    free(sorting_array);
    return true;
}

