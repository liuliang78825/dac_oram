

#include "partition.h"


extern int partition_num;
extern int tree_size;
extern int block_size;
extern int bar_size;
extern int Z;
extern int user_num;
extern int memory_access_circle;
extern int request_num;
extern int region_num;



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

void Partial_Shuffle(int *permutation,int *in_memory_list, uint8_t* in_mem_data, uint8_t * old_key, uint8_t *new_key, int phase){
    
    int shuffle_num; // how many partitions need to be shuffle
 //   int *dummy_num = (int*)malloc(region_num*sizeof(int));
    int part_size =tree_size/2 *Z* block_size / region_num ;
 //   int *new_permutation;
 //   int *seq;
    
    int address;
    int pos[2];
    int distance;
printf("123\n");
//    uint8_t *shuffle_data= (uint8_t*)malloc(7 * part_size* sizeof(uint8_t));
    uint8_t *buffer_data = (uint8_t*)malloc( partition_num * block_size);
rand_array(buffer_data, partition_num*block_size);
printf("233\n");
    char file_name[25];
    for(int i=1 ; i <= region_num ; i++){
        // shuffle the i th region     
  //      dummy_num[i] = ((i+region_num-phase)%region_num);
        
        
  //      shuffle_num = partition_num/region_num + (dummy_num[i]+ 1) * part_size;
     //   rand_array(shuffle_data, dummy_num[i] * part_size);
       // memcpy(shuffle_data + dummy_num[i]*part_size, in_mem_data + i*part_size, part_size);
        // do the in-memory shuffle
        

        
        if(i == phase){ // meet the region
            // load blocks from every phase
            for(int j=(phase-1) * partition_num/region_num; j< phase * partition_num/region_num; j++){

                address = phase * (partition_num /region_num) + j;
                File_read_part(buffer_data, j, 0);
              // do some shuffle
                // ???
                // ???
                // ???
                // write back
                File_write_part(buffer_data, j, 0);
            }
            
   //         for(int j= (phase+1)%region_num; j!=phase; j = (j+1)%region_num){
                // remove all other region
   //             get_pos(pos, phase, j, i);
                
    //            for(int k = pos[0]; k< pos[1]; k++){
    //                sprintf(file_name, "%d-%d.txt", j,k);
    //                remove(file_name);
     //           }
     //       }
        }
        else{
            // do the in-memory shuffle
        //    buffer_data= (uint8_t*)malloc( (dummy_num[i]+1) * part_size* sizeof(uint8_t));
        //    seq = (int *) malloc((dummy_num[i]+1) * part_size / block_size* sizeof(int));
        //    new_permutation =(int *) malloc((dummy_num[i]+1) * part_size / block_size* sizeof(int));
        //    shelter_shuffle(shuffle_data,buffer_data , permutation, new_permutation, seq, old_key, new_key, (dummy_num[i]+1)* part_size/block_size);
            
            get_pos(pos, phase, phase, i);
            // write to the temperory partitions
printf("pos: %d-%d\n", pos[0],pos[1]);
            for(int j=pos[0]; j<pos[1];j++){
             //   memcpy(buffer_data, shuffle_data + (j-pos[0])*partition_num*block_size, partition_num*block_size);
              //  memcpy(buffer_data, in_mem_data, partition_num*block_size);
                File_write_part(buffer_data, j, phase);
            }
        }
	
    }
  //  free(shuffle_data);
    free(buffer_data);
}

void Fully_Shuffle(int *permutation,int *in_memory_list, uint8_t* in_mem_data, uint8_t * old_key, uint8_t *new_key){
    
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

//    permutation_buffer[0] = (int*) malloc(partition_num * sizeof(int));
//    permutation_buffer[1] = (int*) malloc(partition_num * sizeof(int));
//    permutation_buffer[2] = (int*) malloc(partition_num * sizeof(int));
    int offset1, offset2;
    for(int i=0; i<partition_num; i++){
        File_read_part(shuffle_data, i, 0);
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
        File_write_part(shuffle_data, i, 0);
    }
    rand_permutation(permutation, bar_size);
    
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
    
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
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
