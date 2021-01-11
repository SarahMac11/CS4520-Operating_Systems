#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/sstring.h"

bool string_valid(const char *str, const size_t length)
{
    // error check parameters are exist
    if(str && length) {
        char *null_char = "\0";     // null terminated character
        str = str + length - 1;     // get last index
        // compare char to null character
        if(strcmp(str, null_char) == 0)
            return true;
    }
    return false;
}

char *string_duplicate(const char *str, const size_t length)
{
    // error check
    if(str && length) {
        char *duplicate = malloc(length*sizeof(char));     // allocate space for duplicate string
        // check space was allocated for string and copy memory works
        if(duplicate && memcpy(duplicate, str, length*sizeof(char)))
            return duplicate;                             // return allocated ptr to new string
    }
    return NULL;        // else null
}

bool string_equal(const char *str_a, const char *str_b, const size_t length)
{
    // error check
    if(str_a && str_b && length) {
        // compare string a and b
        if(memcmp(str_a, str_b, length) == 0)
            return true;
    }
    return false;
}   

int string_length(const char *str, const size_t length)
{
    if(str && length)
        return strlen(str);     // return str length
    return -1;                  // else return -1 for invalid string
}

int string_tokenize(const char *str, const char *delims, const size_t str_length, char **tokens, const size_t max_token_length, const size_t requested_tokens)
{
    if(str && delims && str_length && tokens && max_token_length && requested_tokens) {
        size_t h = 0, i = 0, j = 0, k = 0;   
        // check if tokens contain the parsed tokens
        while(h < requested_tokens) {
            if(tokens[h] == NULL)
                return -1;      // return -1 for incorrect token allocation
            h++;
        }
        // while not hitting terminated character
        while(str[i] != '\0') {
            // check if a delimiter was hit (used to split into str into segments)
            if(str[i] == *delims) {
                tokens[j][k] = '\0';    // add a null terminator to end str segment
                j++;
                i++;
                k = 0;
            }
            // set token to the next str index
            tokens[j][k] = str[i];
            k++;
            i++;
        }

        return j + 1;           // j incremented for each delim found, including one at end
    } 
    return 0;       // return 0 for incorrect params
}

bool string_to_int(const char *str, int *converted_value)
{
    char *temp = malloc(sizeof(str));       // temp to hold chars to convert
    if(str && converted_value) {
        int i = 0, j = 0;
        // while not hitting a null terminator
        while(str[i] != '\0') {
            // using ASCII values
            if((int)(*str + i) > 47 && (int)(*str + i) < 58) {
                temp[j] = str[i];       // set temp to str if valid
                j++;
            }
            i++;
        }
        if(i > 10) return false;

        *converted_value = atoi(temp);      // convert temp to int
        if(converted_value)
            return true;
    }
    return false;
}
