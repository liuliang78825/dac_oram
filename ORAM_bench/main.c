
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include "aesni.h"
#include "utilities.h"
#include "file.h"
#include "partition.h"
#include "scheduler.h"

int data_size = 67108864; // how many block in bytes
int tree_size;
int bar_size;
int partition_num;
int tree_ratio = 8;
int block_size = 1024; // 1 kilobyte to byte
int Z = 4;
int request_num = ROB_SIZE; // generate 5000 requests
int key_size = 16; // 16 bytes = 128 bits
int shuffle_period;

int memory_access_cycle_II = 5; // access file every 5 circles
int memory_access_cycle_I = 3; 
int stage_I;
int stage_II;


struct log_t{
    time_t start;
    time_t mem_end;
    time_t io_end;
};


void Compute_Param(){
    tree_size = (data_size/tree_ratio)/block_size/Z;
    bar_size = data_size/block_size;
    partition_num = sqrt_int(bar_size);
    shuffle_period = tree_size *Z/2;
    stage_I = shuffle_period*0.1;
    stage_II = shuffle_period*0.4;
}


int main(int argc, const char * argv[]) {
    
    Compute_Param();

    // key need one 128 bits for tree, one 128 bit for fully shuffle
    uint8_t *tree_key = (uint8_t *)malloc(key_size);
    uint8_t *bar_key = (uint8_t *)malloc(key_size);
    uint8_t *bar_key_new = (uint8_t *)malloc(key_size);

    rand_array(tree_key, key_size);
    
    // e.g
    // permutation[3] = 4 :
    // represent the third block in physical address is the 4th in real address
    // when the data is loaded, permutation[i]<0
    int *permutation = (int *)malloc(bar_size*sizeof(int));
    rand_permutation(permutation, bar_size);
    
    // in-memory data

    int phase = 1;    // start with phase I
    Stash stash;
    stash_init(&stash);
    int *pos_map = (int*)malloc(tree_size * sizeof(int));
    int *in_memory_list = (int*) malloc(shuffle_period*sizeof(int));
    uint8_t *tree = (uint8_t*)malloc( tree_size * Z* (block_size + sizeof(int)) * sizeof(uint8_t));
    tree_init(pos_map, tree, tree_key);
    int N = 0;

    // data for  scheduler
    ROB * rob_table  = (ROB*) malloc(request_num * sizeof(ROB));
    REQ * request = (REQ*) malloc(memory_access_cycle_II *sizeof(REQ));
    scheduler_init(rob_table, request, in_memory_list);
    
    // create files and init them
    FileInit(bar_key);
    
    // create thread for I/O write or read;
    pthread_t read_thread;
    
    // in each access circle, the first 'c' circle is to fetch the missing data in the last circle
    int file_address;
    struct file_input input;
    
    input.data_buffer = (uint8_t*) malloc(block_size *sizeof(uint8_t));
    uint8_t *shuffle_buffer;
    int address_buffer = -1;
    int size = sqrt_int(tree_size/2 * Z);

    // in each shuffle period, we define 3 access stage
    clock_t start = clock();
    
    while(1){
        file_address = schedule(rob_table, request, permutation, in_memory_list, N);
        
        // print permutation
        
// ----------------------  I/O Reading   -----------------------
        if(file_address == -1)
            break;
        input.address = permutation[file_address];
        input.phase = phase;
        pthread_create(&read_thread, NULL, File_read, &input);
// -------------- Stage I: no tree access, only stash access --------------------
        if(N>0 && N<=stage_I){
            stash_access(&stash, pos_map[request[0].tree_address], request[0].data);
        }
        
// --------------- Stage II: small amount of tree accesses ---------------------

        if(N>stage_I && N <=stage_II){
            for(int i=0; i<memory_access_cycle_I; i++){
                tree_access(request[i].data, tree, pos_map, &stash, request[i].tree_address,request[i].op , tree_key);
            }
        }
// --------------- Stage III: large amount of tree accesses ----------------------
        if(N> stage_II && N< shuffle_period){
            for(int i=0; i<memory_access_cycle_II; i++){
                tree_access(request[i].data, tree, pos_map, &stash, request[i].tree_address,request[i].op , tree_key);
            }
        }
        
        pthread_join(read_thread, NULL);
        
        address_buffer =(rand()%(tree_size)); // map size
        permutation[file_address] = (-1) * address_buffer;
        array_decrypt(input.data_buffer, bar_key, block_size);
        stash_insert(input.data_buffer, file_address, &stash);
        in_memory_list[N] = file_address;
        N++;
        
        if(N == shuffle_period){
            printf("begin shuffle\n");
            shuffle_buffer = (uint8_t*) malloc(size *size * block_size* sizeof(uint8_t));
            tree_evict(pos_map, tree, tree_key, &stash, shuffle_buffer, in_memory_list,permutation);
            rand_array(bar_key_new, key_size); // generate new key
            Inmemory_shuffle(shuffle_buffer, in_memory_list, bar_key, bar_key_new);
            Fully_Shuffle(permutation, in_memory_list, shuffle_buffer, bar_key, bar_key_new, phase);
            memcpy(bar_key, bar_key_new, key_size); // remove old key
            N = 0;
        }
        
    }
    clock_t time = clock()-start;
    printf("time using:%lu, scale: %lu\n", time, CLOCKS_PER_SEC);
    
    // clean the allocated memory
    stash_free(pos_map, tree, tree_key, &stash);
    free(input.data_buffer);
    scheduler_deinit(rob_table, request);
    free(tree_key);
    free(bar_key);
    free(pos_map);
    free(tree);
    free(permutation);
    return 0;
}
