//
//  access.c
//  Oram
//
//  Created by Leo on 4/6/19.
//  Copyright © 2019 Liang. All rights reserved.
//

#include "access.h"

extern int partition_num;
extern int tree_size;
extern int block_size;
extern int bar_size;
extern int Z;
extern int user_num;
extern int memory_access_circle;
extern int request_num;

extern int shuffle_period;
extern int memory_access_cycle_II; // access file every 5 circles
extern int memory_access_cycle_I;
extern int stage_I;
extern int stage_II;

extern int region_num;

void Partial_Access(int *N, int file_address, int* permutation, Stash *stash, int *pos_map, REQ * request, int *in_memory_list, uint8_t *tree, uint8_t *tree_key, uint8_t *bar_key , int phase){
    
    int address_buffer = -1;
    pthread_t read_thread;
    struct file_inputs inputs;
    
    int region = permutation[file_address] / (partition_num * partition_num / region_num) + 1;

    inputs.n = (phase + region_num- region)%region_num;
    inputs.address = (int*)malloc((inputs.n+1)*sizeof(int));
    inputs.phase = (int*)malloc((inputs.n+1)*sizeof(int));
    inputs.data_buffer = (uint8_t**)malloc((inputs.n+1)*sizeof(uint8_t*));
    inputs.data_buffer[0] = (uint8_t*) malloc(block_size*sizeof(uint8_t));
    inputs.address[0] = permutation[file_address];
    inputs.phase[0] = 0;
    
    int pos[2];
    int j=1;
    
    for(int i= phase; i != (phase+region_num-inputs.n - 1)%region_num + 1; i = (i+region_num-2) %region_num + 1){
        // generate dummy address
        get_pos(pos, phase, i, region);
        inputs.data_buffer[j] = (uint8_t*) malloc(block_size*sizeof(uint8_t));
        inputs.address[j] = rand()%((pos[1]-pos[0]) * partition_num) + pos[0]*partition_num;
        inputs.phase[j] = i;
        j = j+1;
    }
    pthread_create(&read_thread, NULL, Multi_Files_read, (void *)&inputs);
    // -------------- Stage I: no tree access, only stash access --------------------
    if(*N>0 && *N<=stage_I){
        stash_access(stash, pos_map[request[0].tree_address], request[0].data);
    }
    
    // --------------- Stage II: small amount of tree accesses ---------------------
    
    if(*N>stage_I && *N <=stage_II){
        for(int i=0; i<memory_access_cycle_I; i++){
            tree_access(request[i].data, tree, pos_map, stash, request[i].tree_address,request[i].op , tree_key);
        }
    }
    // --------------- Stage III: large amount of tree accesses ----------------------
    if(*N> stage_II && *N< shuffle_period){
        for(int i=0; i<memory_access_cycle_II; i++){
            tree_access(request[i].data, tree, pos_map, stash, request[i].tree_address,request[i].op , tree_key);
        }
    }
    
    pthread_join(read_thread, NULL);
    address_buffer =(rand()%(tree_size)); // map size
    permutation[file_address] = (-1) * address_buffer;
    array_decrypt(inputs.data_buffer[0], bar_key, block_size);
    stash_insert(inputs.data_buffer[0], file_address, stash);
    in_memory_list[*N] = file_address;
}

void Normal_Access(int *N, int file_address, int* permutation, Stash *stash, int *pos_map, REQ * request, int *in_memory_list, uint8_t *tree, uint8_t *tree_key, uint8_t *bar_key){
    
    struct file_input input;
    input.data_buffer = (uint8_t*) malloc(block_size *sizeof(uint8_t));
    pthread_t read_thread;
    // ----------------------  I/O Reading   -----------------------
    int address_buffer = -1;
    input.phase = 0;
    input.address = permutation[file_address];
    pthread_create(&read_thread, NULL, File_read, &input);
    // -------------- Stage I: no tree access, only stash access --------------------
    if(*N>0 && *N<=stage_I){
        stash_access(stash, pos_map[request[0].tree_address], request[0].data);
    }
    
    // --------------- Stage II: small amount of tree accesses ---------------------
    
    if(*N>stage_I && *N <=stage_II){
        for(int i=0; i<memory_access_cycle_I; i++){
            tree_access(request[i].data, tree, pos_map, stash, request[i].tree_address,request[i].op , tree_key);
        }
    }
    // --------------- Stage III: large amount of tree accesses ----------------------
    if(*N> stage_II && *N< shuffle_period){
        for(int i=0; i<memory_access_cycle_II; i++){
            tree_access(request[i].data, tree, pos_map, stash, request[i].tree_address,request[i].op , tree_key);
        }
    }
    
    pthread_join(read_thread, NULL);
    
    address_buffer =(rand()%(tree_size)); // map size
    permutation[file_address] = (-1) * address_buffer;
    array_decrypt(input.data_buffer, bar_key, block_size);
    stash_insert(input.data_buffer, file_address, stash);
    in_memory_list[*N] = file_address;
}
