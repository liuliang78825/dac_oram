

#include "partition.h"


extern int partition_num;
extern int tree_size;
extern int block_size;
extern int bar_size;
extern int Z;
extern int user_num;
extern int memory_access_circle;
extern int request_num;

void PartInit(uint8_t **partition, Stash *stash, int **pos_map){
    int size = sqrt_int(partition_num);
    partition = (uint8_t**)malloc( size * sizeof(uint8_t*));
    stash = (Stash *)malloc( size * sizeof(Stash *));
    pos_map = (int**)malloc( size * sizeof(int*));
}


void Load_Partition(uint8_t **partition, Stash *stash, int **pos_map, uint8_t *key, uint8_t *input, int pos){
    
    int part_size = (block_size + 4)*tree_size * Z;
    int pos_map_size = tree_size * sizeof(int);
    
    partition[pos] = (uint8_t*) malloc(part_size);

    stash_init(stash+pos);

    pos_map[pos] = (int*) malloc(pos_map_size);
    
    memcpy(pos_map[pos], input, pos_map_size);
    memcpy(partition[pos], input + pos_map_size, part_size);
    uint8_t *temp_map = (uint8_t*)pos_map[pos];
    
    otp_crypto(temp_map, key, pos_map_size);
    otp_crypto(partition[pos], key, part_size);
}

void Write_Partition(uint8_t **partition, Stash *stash, int **pos_map, uint8_t *key, uint8_t *input, int pos){
    
    uint8_t *temp_map = (uint8_t*)pos_map[pos];
    otp_crypto(temp_map, key, tree_size * sizeof(int));
    memcpy(input, temp_map, tree_size * sizeof(int));
    otp_crypto(partition[pos], key, (block_size + 4)*tree_size * Z);
    memcpy(input + tree_size * sizeof(int), partition[pos],  (block_size + 4)*tree_size * Z);
    free(pos_map[pos]);
    free(partition[pos]);
}

void Fully_Shuffle(int *permutation,int *in_memory_list, uint8_t* in_mem_data, uint8_t * old_key, uint8_t *new_key, int phase){
    
    // firstly calculate the evicted data
    
    uint8_t *shuffle_data = (uint8_t *)malloc(partition_num * block_size);
    uint8_t *shelter_data = (uint8_t *)malloc(partition_num * block_size);
    
    
//    int ** info_bucket = (int**)malloc(partition_num * sizeof(int*));
//    for(int i=0; i<partition_num; i++){
//        info_bucket[i] = (int*) malloc(3*partition_num *sizeof(int));
//        info_bucket[i][0] = 1;
//    }
//    int address;
//    for(int i=0; i< tree_size * Z / 2; i++){
//        address = -1*permutation[in_memory_list[i]];
//        permutation[in_memory_list[i]] = address;
//        info_bucket[ address/ partition_num ][ info_bucket[ address/ partition_num ][0]] = address;
//        info_bucket[ address/ partition_num ][0] ++;
//    }
//    int counter = 0;
    
    int **permutation_buffer = (int **) malloc(3*sizeof(int*));
    int *seq = (int*) malloc(partition_num * sizeof(int));

    permutation_buffer[0] = (int*) malloc(partition_num * sizeof(int));
    permutation_buffer[1] = (int*) malloc(partition_num * sizeof(int));
    permutation_buffer[2] = (int*) malloc(partition_num * sizeof(int));
    int offset1, offset2;
    for(int i=0; i<partition_num; i++){
        File_read_part(shuffle_data, i, phase);
        offset1 = i*partition_num;
        offset2 = (i+1)*partition_num;
//        for(int j=0; j<partition_num * partition_num; j++){
//            if(permutation[j] < offset2 && permutation[j] >= offset1){
//                permutation_buffer[1][permutation[j]-offset1] = j;
//                permutation_buffer[0][permutation[j]-offset1] = permutation[j];
//            }
//        }
//        for(int j=1; j<info_bucket[i][0]; j++){
//            memcpy(shuffle_data + (info_bucket[i][j]%partition_num)*block_size, in_mem_data + counter * block_size, block_size);
//            permutation_buffer[0][info_bucket[i][j]] = in_memory_list[counter];
//            counter++;
//        }
        shelter_shuffle(shuffle_data, shelter_data, permutation_buffer[0], permutation_buffer[2], seq, old_key, new_key, partition_num);
//        for(int j=0; j<partition_num; j++){
//            permutation[permutation_buffer[1][j]] = permutation_buffer[0][j];
//        }
        File_write_part(shuffle_data, i, phase);
    }
    rand_permutation(permutation, bar_size);
    
    free(permutation_buffer[0]);
    free(permutation_buffer[1]);
    free(permutation_buffer[2]);
    free(permutation_buffer);
    
    free(shelter_data);
    free(shuffle_data);
    free(seq);
}


void Inmemory_shuffle(uint8_t * data, int *permutation, uint8_t *old_key, uint8_t *new_key){
    
    int size = sqrt_int(tree_size/2 * Z);
    uint8_t *shelter_data = (uint8_t *)malloc(size * block_size *sizeof(uint8_t));
    int *permutation_buffer = (int*)malloc(size * sizeof(int));
    int *new_permutation = (int*)malloc(size * size * sizeof(int));
    int *seq = (int*)malloc(size*sizeof(int));
    
    uint8_t *data_buffer = (uint8_t *)malloc(size * size * block_size *sizeof(uint8_t));
    
    // phase I
    
    for(int i=0; i<(size -1); i++){
        for(int j=0; j<(size-1); j++){
            memcpy(data_buffer + (i*size +j) * block_size, data + (j*size + i)*block_size, block_size);
            new_permutation[i*size +j] = permutation[j*size +i];
        }
        shelter_shuffle(data_buffer + i*size, shelter_data, new_permutation, permutation_buffer,seq, old_key, new_key, size);
    }
    
    // phase II
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            memcpy(data + (i*size +j) * block_size, data_buffer + (j*size + i)*block_size, block_size);
            permutation[i*size +j] = new_permutation[j*size +i];
        }
        shelter_shuffle(data + i*size, shelter_data, permutation, permutation_buffer, seq,old_key, new_key, size);
    }
    
    free(shelter_data);
    free(data_buffer);
    free(new_permutation);
    
}

void shelter_shuffle(uint8_t *data, uint8_t *shelter_data, int* permutation, int *new_permutation,int *seq, uint8_t *old_key, uint8_t *new_key, int size){
    
    int temp;
    for(int i=0; i<size; i++){
        seq[i] = i;
    }
    for(int i=0; i<size; i++){
        temp = rand()%(size-i);
        new_permutation[i] = seq[temp];
        seq[temp] = seq[size-i-1];
        seq[size-i-1] = new_permutation[i];
    }
    
    for(int i=0; i<size; i++){
        memcpy(shelter_data + new_permutation[i] * block_size, data + i*block_size, block_size);
        array_decrypt(shelter_data + new_permutation[i], old_key, block_size);
        array_encrypt(shelter_data + new_permutation[i], new_key, block_size);
//        new_permutation[i] = permutation[new_permutation[i]];
    }
//    memcpy(permutation, new_permutation, sizeof(int) * size);
    memcpy(data, shelter_data, size*block_size);
    
}
