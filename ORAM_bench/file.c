
#include "file.h"
extern int tree_size;
extern int bar_size;
extern int tree_size;
extern int partition_num;
extern int block_size;
extern int Z;
extern int user_num;
extern int memory_access_circle;
extern int request_num;

void FileInit(uint8_t* bar_key){
    uint8_t *data_block= (uint8_t *)malloc(block_size * sizeof(uint8_t));
    for(int i=0; i< partition_num;i++){
        int phase = 1;
        FILE *fd = NULL;
        char file_name[15] = "";
        sprintf(file_name, "data/%d-%d", phase, i);
        fd = fopen(file_name, "w+");
        for(int j=0; j<partition_num;j++){
            rand_array(data_block, block_size);
            fwrite(data_block, 1, block_size, fd);
        }
        fclose(fd);
    }
}

void File_write_part(uint8_t *data, int address, int phase){
    FILE *fd;
    char file_name[20];
//    for(int i=0; i<partition_num; i++){
//        sprintf(file_name, "data/%d-%d", phase, i);
//        fd = fopen(file_name,"a");
//        fwrite(data + i*block_size, 1, block_size, fd);
//        fclose(fd);
//    }
clock_t start = clock();
    sprintf(file_name, "data/%d-%d", phase, address);
    fd = fopen(file_name, "w+");
    fwrite(data, 1, block_size*partition_num, fd);
    fclose(fd);
fd = fopen("log3", "a");
fprintf(fd, "%d\n", clock()-start);
fclose(fd);
}

void File_read_part(uint8_t *data, int address, int phase){
    FILE *fd;
    char file_name[20];
clock_t start = clock();
    sprintf(file_name, "data/%d-%d", phase, address);
    fd = fopen(file_name, "r");
    fread(data, 1, block_size*partition_num, fd);
    remove(file_name);
    fclose(fd);
fd = fopen("log2", "a");
fprintf(fd, "%d\n", clock()-start);
fclose(fd);
}

void *File_read(void *input_t){
clock_t start = clock();
    FILE_INPUT *input = (FILE_INPUT*) input_t;
    FILE *fd = NULL;
    char file_name[15] = "";
    int file_address = input->address/partition_num;

printf("%d\n",file_address);
    int offset = (input->address - file_address * partition_num) * block_size;
    sprintf(file_name, "data/%d-%d", input->phase, file_address);
    fd = fopen(file_name, "r");
    fseek(fd, offset, SEEK_SET);
    fread(input->data_buffer, 1, block_size, fd);
    
    fclose(fd);
 
    fd = fopen("log", "a");

    fprintf(fd, "%d\n", clock()-start);
    fclose(fd);
    return NULL;
}

void *File_write(void *input_t){
    FILE_INPUT *input = (FILE_INPUT*) input_t;
    FILE *fd = NULL;
    char file_name[15] = "";
    int file_address = input->address/partition_num;
    int offset = (input->address - file_address * partition_num) * block_size;
    sprintf(file_name, "data/%d-%d", input->phase, file_address);
    fd = fopen(file_name, "w");
    fseek(fd, offset, SEEK_SET);
    fwrite(input->data_buffer, 1, block_size, fd);
    fclose(fd);
    return NULL;
}
