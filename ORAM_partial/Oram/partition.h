
#ifndef partition_h
#define partition_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include "utilities.h"
#include "aesni.h"
#include "scheduler.h"
#include "file.h"

#endif /* partition_h */

void Write_Partition(uint8_t **partition, Stash *stash, int **pos_map, uint8_t *key, uint8_t *input, int pos);

void Load_Partition(uint8_t **partition, Stash *stash, int **pos_map, uint8_t *key, uint8_t *input, int pos);

void PartInit(uint8_t **partition, Stash *stash, int **pos_map);

void Fully_Shuffle(int *permutation,int *in_memory_list, uint8_t* in_mem_data, uint8_t * old_key, uint8_t *new_key);

void Inmemory_shuffle(uint8_t * data, int *permutation, uint8_t *old_key, uint8_t *new_key);

void shelter_shuffle(uint8_t *data, uint8_t *shelter_data, int* permutation, int *new_permutation,int *seq, uint8_t *old_key, uint8_t *new_key, int size);

void Partial_Shuffle(int *permutation,int *in_memory_list, uint8_t* in_mem_data, uint8_t * old_key, uint8_t *new_key, int phase);
