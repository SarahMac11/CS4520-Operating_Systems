#include "../include/structures.h"
#include <stdio.h>

int compare_structs(sample_t* a, sample_t* b)
{
    // error check params
    if(!a || !b)
        return 0;
    if(a->a == b->a && a->b == b->b && a->c == b->c)
        return 1;
    return 0;
}

void print_alignments()
{
    printf("Alignment of int is %zu bytes\n",__alignof__(int));
    printf("Alignment of double is %zu bytes\n",__alignof__(double));
    printf("Alignment of float is %zu bytes\n",__alignof__(float));
    printf("Alignment of char is %zu bytes\n",__alignof__(char));
    printf("Alignment of long long is %zu bytes\n",__alignof__(long long));
    printf("Alignment of short is %zu bytes\n",__alignof__(short));
    printf("Alignment of structs are %zu bytes\n",__alignof__(fruit_t));
}

int sort_fruit(const fruit_t* a,int* apples,int* oranges, const size_t size)
{
    // error check params
    if(!a || !apples || !oranges || size <= 0)
        return -1;
    //#define ORANGE 0
    //#define APPLE 1
    size_t s = 0;
    for(size_t i = 0; i < size; i++) {
        if(IS_APPLE(a + i)) {
            (*apples)++;        // inc apples
            s++;                // inc size
        } else if(IS_ORANGE(a + i)) {
            (*oranges)++;
            s++;
        }
        else
            return -1;          // else error
    }

    if(s == size)
        return s;
    return 0;
}

int initialize_array(fruit_t* a, int apples, int oranges)
{
    // error check params
    if(!a || apples <= 0 || oranges <= 0)
        return -1;

    for(int i = 0; i < apples; i++) {
        // add type of fruit to array a
        a->type = APPLE;
        a++;
    }

    for(int j = 0; j < oranges; j++) {
        a->type = ORANGE;
        a++;
    }

    return 0;
}

int initialize_orange(orange_t* a)
{
    if(!a)
        return -1;
    // create orange struct
    a->type = ORANGE;
    a->weight = 1 + rand() % 50;    // rand weight between 1 - 50
    a->peeled = rand() % 1;     // rand peeled between 0 and 1

    return 0;

}

int initialize_apple(apple_t* a)
{
    if(!a)
        return -1;
    // create apple struct
    a->type = APPLE;
    a->weight = rand() % 50;
    a->worms = rand() % 10;

    return 0;
}
