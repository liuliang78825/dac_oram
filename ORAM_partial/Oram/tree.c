
#include "tree.h"

extern int partition_num;
extern int tree_size;
extern int block_size;
extern int Z;
extern int user_num;
extern int memory_access_circle;
extern int request_num;
extern int key_size;

int max_level;

void stash_access(Stash *stash, int address, uint8_t *data){
    Stash_Block *p = stash->start;
    for(int i=0; i<stash->size; i++){
        if(p->ID == address){
            memcpy(data, p->data, block_size);
            break;
        }
        else
            p=p->next;
    }
}


void stash_insert(uint8_t *data, int ID, Stash *stash){
    Stash_Block *new_block = (Stash_Block*) malloc(sizeof(Stash_Block));
    new_block->ID = ID;
    new_block->next = NULL;
    new_block->data = (uint8_t*)malloc(block_size);
    memcpy(new_block->data, data, block_size);
    
    if(stash->size == 0){
        stash->start = new_block;
        stash->end = new_block;
    }
    else{
        stash->end->next = new_block;
        stash->end = new_block;
    }
    stash->size += 1;
    
}

void stash_init(Stash *stash){
    stash->start = NULL;
    stash->end = NULL;
    stash->size = 0;
}

Stash_Block* stash_erase(Stash *stash, int pos){
    Stash_Block *temp = stash->start;
    Stash_Block *temp2 = temp;
    if(pos == 0){
        stash->start = stash->start->next;
        temp2 = stash->start;
    }
    else if(pos == stash->size -1){
        
    }
    else{
        for(int i=0; i<pos; i++){
            temp2 = temp;
            temp = temp->next;
        }
        temp2->next = temp2->next->next;
        temp2 = temp2->next;
    }
    if(temp2->next == NULL){
        stash->end = temp2;
    }
    stash->size = stash->size - 1;
    free(temp->data);
    free(temp);
    return temp2;
}


int get_node(int level, int leaf){
    //                    0
    //          1                      2
    //     3         4         5               6
    // 7      8   9     10  11    12       13      14
    // 0      1   2      3   4     5        6       7
    
    leaf = leaf >> (max_level - level);
    
    return pow_int(level) + leaf - 1;
}

void tree_access(uint8_t *data, uint8_t *tree, int *pos_map, Stash *stash, int address, int isRead, uint8_t *key){
    
    int x = pos_map[address];
    pos_map[address] = rand()%((tree_size)/2);

    uint8_t *temp_bucket = (uint8_t*) malloc((block_size + 4) * Z);
    int level = log_int(tree_size);
    int pos;
    int temp_x = x;
    int *ID;
    int aa;
    Stash_Block * temp_block;
    
    for(int i=level; i > 0; i--){
        pos = x >> (level-i);
        memcpy(temp_bucket, tree + i * (block_size + 4) * Z, (block_size + 4) * Z);
        
        array_decrypt(temp_bucket, key, (block_size + 4) * Z);
        
        for(int j=0; j<Z; j++){
            ID = (int*)(temp_bucket + j * (block_size + 4));
            aa = *ID;

            if(*ID >=0 && *ID <= tree_size){
                temp_block = (Stash_Block*)malloc(sizeof(Stash_Block));
                temp_block->ID = *ID;
                temp_block->data = (uint8_t*)malloc(block_size);
                temp_block->next = NULL;
                memcpy(temp_block->data, temp_bucket + j * (block_size + 4) + 4, block_size);
                if(stash->start == NULL){
                    stash->start = temp_block;
                    stash->end = temp_block;
                }
                else{
                    stash->end->next = temp_block;
                    stash->end = temp_block;
                }
                stash->size++;
            }
        }
    }

    
//    printf("stash size: %d",stash->size);
    // write or read data in stash
    pos = -1;
    
    for(temp_block = stash->start; temp_block != stash->end; temp_block = temp_block->next){
        if(temp_block->ID == address){
            break;
        }
    }
    
    if(isRead == 1){
        // read data from stash;
        // if not exitst, create one
        if(pos == -1){
//            temp_bucket = (uint8_t*) malloc(block_size);
//            for(int i=0; i<block_size; i++)
//                temp_bucket[i] = 0;
//            memcpy(data, temp_bucket, block_size);
//            stash_insert(temp_bucket, address, stash);
//            free(temp_bucket);
        }
        else{
            memcpy(data, temp_block->data, block_size);
        }
    }
    else{
        // write data to stash
        if(pos == -1){
            stash_insert(data, address, stash);
        }
        else{
            memcpy(temp_block->data, data, block_size);
        }
    }
    // create a blank block
    uint8_t *blank_block = (uint8_t*) malloc(block_size);
    for(int i=0; i<block_size; i++){
        blank_block[i] = 0;
    }
    // write stash to path
    Stash_Block *temp2;
    int num;
    int buffer;
    int counter = 0;
    for(int i=level; i>0; i--){
        temp_block = stash->start;
        temp2 = stash->start;
        num = 0;
        pos = x >> (level - i);
        counter = 0;
        for(;num < Z && temp_block!= NULL;){
            counter ++;
           // if(pos_map[temp_block->ID]>>(level - i) == pos){
            if(1){
                buffer = temp_block->ID;
                memcpy(temp_bucket + num*(block_size + 4), &buffer, 4);
                memcpy(temp_bucket + num*(block_size + 4) + 4, temp_block->data, block_size);
                num++;
                stash->size--;
                if(temp_block == stash->start){
                    if(temp_block == stash->end){
                        stash->start = NULL;
                        stash->end = NULL;
                        free(temp_block->data);
                        free(temp_block);
                        break;
                    }
                    else{
                        stash->start = temp_block->next;
                        free(temp_block->data);
                        free(temp_block);
                        temp_block = stash->start;
                        temp2 = stash->start;
                        continue;
                    }
                }
                if(temp_block == stash->end){
                    stash->end = temp2;
                }
                temp2->next = temp_block->next;
                free(temp_block->data);
                free(temp_block);
                temp_block = temp2->next;
                if(temp2 == stash->end)
                    break;
            }
            else{
                temp2 = temp_block;
                if(temp_block == stash->end)
                    break;
                temp_block = temp_block->next;
            }
        }
        for(; num<Z; num++){
            buffer = -1;
            memcpy(temp_bucket + num*(block_size + 4), &buffer, 4);
            memcpy(temp_bucket + num*(block_size + 4) + 4, blank_block, block_size);
        }
        
        array_encrypt(temp_bucket, key, (block_size + 4) * Z);
        memcpy(tree + pos * (block_size + 4) * Z, temp_bucket, (block_size + 4) * Z);
    }
//    printf("-> %d\n",stash->size);
    
    
}

void tree_init(int *pos_map, uint8_t *part, uint8_t *key){
    
    int temp, level;
    
    for(int i=0; i<tree_size; i++){
        pos_map[i] = rand()%(tree_size/2);
    }
    
    max_level = log_int(block_size);
    
    int bucket_size = (block_size+4)*Z;
    int ID;
    uint8_t *encry_data = (uint8_t *)malloc((block_size+4)*Z);
    
    for(int i=0; i<tree_size; i++){
        for(int j=0; j<Z; j++){
            ID = -1;
            memcpy(encry_data + j*(block_size+4), &ID, 4);
            rand_array(encry_data +  j*(block_size+4) + 4, block_size);
        }
        array_encrypt(encry_data, key, bucket_size);
        memcpy(part + i*bucket_size, encry_data, bucket_size);
    }
    //                    0
    //          1                      2
    //          0                      1
    //     3         4         5               6
    //     0         1         2               3
    // 7      8   9     10  11    12       13      14
    // 0      1   2      3   4     5        6       7
//    for(int j=0; j<tree_size; j++){
//        temp = pos_map[j];
//        level = log_int(temp);
//        temp = temp - pow_int(level) + 1;
//        for(int k=level; k< log_int(tree_size) - 1; k++){
//            temp = temp*2 + rand()%2;
//        }
//        pos_map[j] = temp;
//    }
    free(encry_data);
}

void tree_evict(int *pos_map, uint8_t *tree, uint8_t *key, Stash *stash,uint8_t *data_buffer, int * in_memory_list, int *permutation){
    int pos = 0;

    uint8_t *new_key = (uint8_t*)malloc(key_size * sizeof(uint8_t));
    rand_array(new_key, key_size);
    int *buffer_map = (int*) malloc(tree_size/2 * sizeof(int));
    // load the map (tree ID to permutation ID)
    for(int i=0; i<tree_size/2; i++){
  //      buffer_map[-1* permutation[in_memory_list[i]]] = in_memory_list[i];
    }
    // evict the stash
    for(Stash_Block *temp_block = stash->start; temp_block!=stash->end;  ){
        memcpy(data_buffer + pos * block_size , temp_block->data, block_size);
    //    in_memory_list[pos] = buffer_map[temp_block->ID];
        free(temp_block->data);
        stash->start = temp_block->next;
        free(temp_block);
        temp_block = stash->start;
        stash->size--;
    }
    // check the map
    int *ID;
    uint8_t *temp_bucket = (uint8_t*) malloc((block_size+4)*Z);
    
    int aa;
    for(int i=0; i<tree_size; i++){
        memcpy(temp_bucket, tree + i * (block_size+4)*Z, (block_size+4)*Z);
        array_decrypt(temp_bucket, key, (block_size+4)*Z);
        for(int j=0; j<Z; j++){
            ID = (int*)(temp_bucket + j*(block_size+4));
            aa = *ID;
            if(aa >= 0 && aa < tree_size *Z/2 ){
                memcpy(data_buffer + pos * block_size ,temp_bucket + j*(block_size+4) + 4, block_size);
        //        in_memory_list[pos] = buffer_map[aa];
                pos++;
                *ID=-1;
            }
        }
    }
    
  //  free(tree);
}

void stash_free(int *pos_map, uint8_t *tree, uint8_t *key, Stash *stash){
    for(Stash_Block *temp_block = stash->start; temp_block!=stash->end;  ){
        free(temp_block->data);
        stash->start = temp_block->next;
        free(temp_block);
        temp_block = stash->start;
        stash->size--;
    }
}
