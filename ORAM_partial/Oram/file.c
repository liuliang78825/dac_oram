
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

extern int shuffle_mode;
extern int region_num;

void get_pos(int *pos, int phase, int target, int region){
    int part_size =tree_size/2 *Z / region_num ;
    int distance = (region + region_num - target) % region_num;
    pos[0] = distance*(distance-1)/2 * part_size / partition_num;
    pos[1] = distance*(distance + 1)/2 * part_size / partition_num;
    
}

void FileInit(uint8_t* bar_key){
    uint8_t *data_block= (uint8_t *)malloc(block_size * partition_num * sizeof(uint8_t));
    int phase = 0;
    rand_array(data_block, block_size * partition_num);
    for(int i=0; i< partition_num;i++){
        File_write_part(data_block, i, phase);
    }
    
    
    int part_size =tree_size/2 *Z / region_num ;
    int distance = 1;
    int pos[2];
    
    if(shuffle_mode == 2){
        for( phase = 1; phase != 2 ; phase=(phase + region_num - 2 )%region_num + 1){
            // from 1, 3, 6: 0~6, 1~6, 3~6
            pos[0] = (distance*(distance - 1)/2) * part_size /partition_num;
            pos[1] = region_num*(region_num - 1) / 2 * part_size / partition_num;
            
            for(int i=pos[0]; i< pos[1];i++){
                File_write_part(data_block, i, phase);
            }
            distance++;
        }
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
  //  remove(file_name);
    fclose(fd);
fd = fopen("log2", "a");
fprintf(fd, "%d\n", clock()-start);
fclose(fd);
}

void *File_read(void *input_t){
    FILE_INPUT *input = (FILE_INPUT*) input_t;
    FILE *fd = NULL;
    char file_name[15] = "";
    int file_address = input->address/partition_num;
    int offset = (input->address - file_address * partition_num) * block_size;
    sprintf(file_name, "data/%d-%d", input->phase, file_address);
printf("file: %s\n",file_name);
    fd = fopen(file_name, "r");
    fseek(fd, offset, SEEK_SET);
    fread(input->data_buffer, 1, block_size, fd);
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


void *Multi_Files_read(void *inputs_t){
clock_t start = clock();
    FILE_INPUTS *inputs = (FILE_INPUTS*) inputs_t;
    FILE *fd = NULL;
printf("%file: %d read",inputs->n + 1);
    char file_name[15] = "";
    int file_address;
    int offset;
    for(int i=0; i<(inputs->n + 1); i++){
        file_address = inputs->address[i]/partition_num;
        offset = (inputs->address[i] - file_address * partition_num) * block_size;
        sprintf(file_name, "data/%d-%d", inputs->phase[i], file_address);
        fd = fopen(file_name, "r");
        fseek(fd, offset, SEEK_SET);
        fread(inputs->data_buffer[i], 1, block_size, fd);
        fclose(fd);
    }
    fd = fopen("log", "a");

    fprintf(fd, "%d\n", clock()-start);
    fclose(fd);
    return NULL;
}
