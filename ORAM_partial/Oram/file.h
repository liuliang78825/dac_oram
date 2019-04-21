

#ifndef file_h
#define file_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utilities.h"
#include "aesni.h"
#include "tree.h"



typedef struct file_input{
    uint8_t* data_buffer;
    int address;
    int phase;
} FILE_INPUT;

typedef struct file_inputs{
    uint8_t** data_buffer;
    int *address;
    int *phase;
    int n;
} FILE_INPUTS;

#endif /* file_h */

void get_pos(int *pos, int phase, int target, int region);

void FileInit(uint8_t* bar_key);

void File_read_part(uint8_t *data, int address, int phase);

void File_write_part(uint8_t *data, int address, int phase);

void *File_read(void *input);

void *File_write(void *input);

void *Multi_Files_read(void *inputs_t);
