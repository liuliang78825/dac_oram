
#include "scheduler.h"

extern int partition_num;
extern int tree_size;
extern int block_size;
extern int Z;
extern int user_num;
extern int request_num;
extern int bar_size;


extern int shuffle_period;
extern int memory_access_cycle_I; // access file every 5 circles
extern int memory_access_cycle_II; 
extern int memory_access_cycle_III; // access file every 5 circles
extern int memory_access_cycle_IV; 

extern int stage_1;
extern int stage_2;
extern int stage_3;
extern int stage_4;


void scheduler_init(ROB *rob_table, REQ *request, int* in_memory_list){
    //generate request
    srand(clock());
    int *data_int;
    for(int i=0; i<request_num;i++){
        rob_table[i].finished=-1;
        rob_table[i].op = rand()%2;
        rob_table[i].address = rand()%((tree_size - 1));
        rob_table[i].data = (uint8_t*)malloc(block_size);
        rob_table[i].signal = SIG_START;
        data_int = (int*)rob_table[i].data;
        if(rob_table[i].op == OP_READ){ // read
            for(int j=0; j<block_size/4;j++){
                data_int[j] = 0;
            }
        }else{
            for(int j=0; j<block_size/4;j++){
                data_int[j] = rand();
            }
        }
    }
    for(int i=0; i<memory_access_cycle_IV; i++){
        request[i].data = (uint8_t *)malloc(block_size*sizeof(uint8_t));
    }
    for(int i=0; i<shuffle_period; i++){
        in_memory_list[i] = -1;
    }
}

int schedule(ROB *rob_table, REQ *request, int * permutation, int *in_memory_list, int N){
    srand(clock());
    
    int load_span = 1;
    int access_cycle = 1;
    int j = 0;
    int pos;
    int memory_pos = 0;
    int io_pos = -1;
    
    if(N == 0){
        // first cicle, no memory access
        
        for(int i=0; i<ROB_SIZE-1; i++){
            if(rob_table[i].finished == 0){
                return rob_table[i].address;
            }
        }
    }
    if(N<= stage_1){
        access_cycle = 1;
        load_span = 3;
    }
    if(N>stage_1 && N<=stage_2){
        access_cycle = memory_access_cycle_I;
        load_span = memory_access_cycle_I * 3;
    }
    if(N>stage_2 &&N<=stage_3){
        access_cycle = memory_access_cycle_II;
        load_span = memory_access_cycle_I * 3;
    }
    if( N>stage_3 && N<=stage_4){
        access_cycle = memory_access_cycle_III;
        load_span = memory_access_cycle_I * 3;
    }
    if(N>stage_4 &&N <= shuffle_period){
        access_cycle = memory_access_cycle_IV;
        load_span = memory_access_cycle_II * 3;
    }
    
    // pop the old request
    for(int i=0; j < access_cycle && i < ROB_SIZE; i++){
        if(rob_table[i].finished >= 0 ){
            if(rob_table[i].signal != SIG_START){
                if(rob_table[i].op == OP_READ){
                    memcpy(rob_table[i].data, request[rob_table[i].finished].data, block_size);
                }
                rob_table[i].finished = -2;
                printf("Job %d finished\n", i);
                j++;
            }
        }
    }
    
    // if shuffle period comes, do not sceduler new
    if(N >= shuffle_period){
        return 0;
    }
    
    // generate dummy
    for(int i=0; i<access_cycle;i++){
        request[i].tree_address = rand()%tree_size;
        request[i].op = OP_READ;
    }
   
    
    // schedule new load request
    
    j = 0;
    for(int i=0; memory_pos <= load_span && i<ROB_SIZE; i++){
        
        if(rob_table[i].finished == -1){
            pos = permutation[rob_table[i].address];
            if( pos < 0 && j < access_cycle){
                // already in memory schedule a request
                printf("N : %d, schedule request:%d\n",N, i);
                request[j].op = rob_table[i].op;
                request[j].tree_address = -1* permutation[rob_table[i].address];
                if(rob_table[i].op == OP_WRITE){
                    memcpy(request[j].data, rob_table[i].data, block_size);
                }
                rob_table[i].finished = j;
                rob_table[i].signal = pos;
                j++;
            }
            else if(pos >0){
                // not in memory, schedule a read
                if(io_pos == -1){
                    io_pos =  rob_table[i].address;
                }
            }
            memory_pos++;
        }
    }
    int rand_num;
    // if no IO need, schedule dummy
    if(io_pos == -1){
        while(1){
            rand_num =rand()%bar_size;
            if(permutation[rand_num] >0)
                break;
        }
        io_pos = rand_num;
    }
    // detect if all task is finished
    int is_finish = 0;
    for(int i=0; i< ROB_SIZE; i++){
        if(rob_table[i].finished == -1){
            is_finish++;
            break;
        }
    }
    if(rob_table[ROB_SIZE-2].finished != -1)
        is_finish = 0;
    if(is_finish == 0){
        // all finish
        return -1;
    }
    else{
        return io_pos;
    }
}


void scheduler_deinit(ROB *rob_table, REQ *request){
    for(int i=0; i<request_num; i++){
        free(rob_table[i].data);
    }
    free(rob_table);
    for(int i=0; i<memory_access_cycle_II; i++){
        free(request[i].data);
    }
    free(request);

}
