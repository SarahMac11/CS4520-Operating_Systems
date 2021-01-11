#ifndef STRUCTURES_H
#define STRUCTURES_H

#ifdef __cplusplus
	extern "C" {
#endif

#define ORANGE 0
#define APPLE 1
#define IS_ORANGE(a) ((a)->type == ORANGE)
#define IS_APPLE(a)  ((a)->type == APPLE)
#include <stdlib.h>

// Data Alignment:
// How a language/computer aligns data, dont to improve performance
// Additional Links:
// https://en.wikipedia.org/wiki/Data_structure_alignment#Typical_alignment_of_C_structs_on_x86
// Can be obtained for any data type (structs, pointers, primitives) using the __alignof__ operator e.g. __alignof__(int);
// Typically the alignment is the same as the size returned by size_of e.g. a char is 1-byte and is 1-byte aligned
// See more about __alignof__ here: https://gcc.gnu.org/onlinedocs/gcc/Alignment.html

// Padding:
// How a language/computer corrects misaligned data, wasted space used to regain alignment (Like placing a book under a table leg.)
// Additional Links:
// https://en.wikipedia.org/wiki/Data_structure_alignment#Data_structure_padding
// Note: the sizeof operator includes the padded bytes

// The size of this struct is less than ideal
// Data is in poor order resulting in padding for Data Alignment
// (Assuming typical data sizes)
typedef struct bad
{
	char a;			// this variable is at offset 0
	long long b;	// this variable is at offset 1, requires alignment 8, 7 bytes of padding added in order to comply
	char c;			// this variable is at offset 16, requires no additional padding.
	int d;			// this variable is at offset 17, requires alignment 4, 3 bytes of additional padding in order to comply
}bad_t;
// Assuming the data size is 14 bytes, the padding is 10 bytes. So sizeof(BAD) would be 24 bytes. Almost doubled in size.

// Generally best to do largest -> smallest members
typedef struct good
{
	long long b;	// this variable is at offset 0
	int d;			// this variable is at offset 8, no padding required
	char a;			// this variable is at offset 16, no padding required as char is 1-byte aligned
	char c;			// this variable is at offset 17, no padding required as char is 1-byte aligned
}good_t;
// Assuming the data size is 14 bytes (same data types). No padding so sizeof(GOOD) would be 14 bytes. We saved 10 bytes!
// Since this padding is repeated per struct we can fit many more structs in the same space in comparison to the BAD struct.

// Structs are padded not only between variables but at the end of structs as well as structs themselves have Data Alignment
// You can check the data alignment of structs by using the __alignof__ operator on one, e.g. __alignof__(GOOD);
// Assuming Structs have an alignment of 4 there will be three byte of padding at the end of the following struct (to make the size an even number).
typedef struct sample
{
	long long a;
	long long b;
	char c;
}sample_t;

// Padding can be useful, e.g. to obtain some degree of polymorphism while using different data sets.
// Note that these prevents you from using things such as enums which have no guaranteed size (Compiler may choose a smaller size so long as it holds all your values).
// Additionally you are dependant on the sizes of your data types in order to determine your padding.

typedef struct orange
{
	int type;
	int weight;
	short peeled;
}orange_t;

typedef struct apple
{
	int type;
	int weight;
	int worms;
}apple_t;

typedef struct fruit
{
	int type;
	char padding[8];
}fruit_t;

// Purpose: Simple function to show the alignment of different variables on standard output.
// Receives: Nothing
// Returns: Nothing
void print_alignments();

// Purpose: Compares two SAMPLE structs member-by-member
// Receives: struct_a - first struct to compare
//			 struct_b - second struct to compare
// Returns: 1 if the structs match, 0 otherwise.
int compare_structs(sample_t* a, sample_t* b);

// Purpose: Categorizes fruits into apples and oranges
// Receives:  fruit_t* a - pointer to an array of fruits,
//			  int* apples - pointer to apples pass-back address,
//			  int* oranges - pointer to oranges pass-back address,
//			  const size_t size - size of array
// Returns: The size of the array, -1 if there was an error.
int sort_fruit(const fruit_t* a,int* apples,int* oranges, const size_t size);

// Purpose: Initializes an array of fruits with the specified number of apples and oranges
// Receives: fruit_t* a - pointer to an array of fruits
//						int apples - the number of apples
//						int oranges - the number of oranges
// Returns: -1 if there was an error, 0 otherwise.
int initialize_array(fruit_t* a, int apples, int oranges);

// Purpose: Initializes an orange_t struct
// Receives: orange_t* - pointer to an orange_t struct
// Returns: -1 if there was an error, 0 otherwise.
int initialize_orange(orange_t* a);

// Purpose: Initializes an apple_t struct
// Receives: apple_t* - pointer to an apple_t struct
// Returns: -1 if there was an error, 0 otherwise.
int initialize_apple(apple_t* a);

#ifdef __cplusplus
	}
#endif

#endif //STRUCTURES_H