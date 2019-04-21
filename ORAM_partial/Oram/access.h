//
//  access.h
//  Oram
//
//  Created by Leo on 4/6/19.
//  Copyright © 2019 Liang. All rights reserved.
//

#ifndef access_h
#define access_h

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include "partition.h"
#include "scheduler.h"
#include "tree.h"

void Normal_Access(int *N, int file_address, int* permutation, Stash *stash, int *pos_map, REQ * request, int *in_memory_list, uint8_t *tree, uint8_t *tree_key, uint8_t *bar_key);

void Partial_Access(int *N, int file_address, int* permutation, Stash *stash, int *pos_map, REQ * request, int *in_memory_list, uint8_t *tree, uint8_t *tree_key, uint8_t *bar_key, int phase);
#endif /* access_h */
