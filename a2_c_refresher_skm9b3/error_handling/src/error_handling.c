#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "../include/error_handling.h"

int create_blank_records(Record_t **records, const size_t num_records)
{
    if(*records != NULL || num_records <= 0 || records == NULL)
        return -1;      // error check params

    *records = (Record_t*) malloc(sizeof(Record_t) * num_records);
    if(*records == NULL)
        return -2;      // bad memory allocation

    memset(*records,0,sizeof(Record_t) * num_records);
    return 0;   
}

int read_records(const char *input_filename, Record_t *records, const size_t num_records) {

    if(input_filename == NULL || records == NULL || num_records < 1)
        return -1;      // error check params
    int fd = open(input_filename, O_RDONLY);
    if(fd == -1)
        return -2;      // bad filename

    ssize_t data_read = 0;
    for (size_t i = 0; i < num_records; ++i) {
        data_read = read(fd,&records[i], sizeof(Record_t)); 
        if(data_read <= -1)     // 0 for EOF and -1 on error 
            return -3;  // bad read request
    }
    return 0;
}

int create_record(Record_t **new_record, const char* name, int age)
{
    if((!new_record || (*new_record) || !name || name[0] == '\0' || name[0] == '\n' || strlen(name) < MAX_NAME_LEN - 1 || age < 0 || age > 200)) {
        //
        /*if(!new_record || (*new_record) || !name || name[0] == '\0' || !strcmp("\n", name) || name[0] == '\n' || strlen(name) < MAX_NAME_LEN - 1 || strlen(name)>50 || age < 0 || age > 200) { */
        printf("fail params");
        return -1;          // bad params
    }

    *new_record = (Record_t*) malloc(sizeof(Record_t));
    if(*new_record == NULL) {
        printf("fail alloc");
        return -2;      // bad allocation
    }

    memcpy((*new_record)->name,name,sizeof(char) * strlen(name));
    (*new_record)->name[MAX_NAME_LEN - 1] = 0;  
    (*new_record)->age = age;
    return 0;

    }

