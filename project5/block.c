#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int BLOCK_SIZE = 4096; // 4KB

unsigned char *bread(int block_num, unsigned char *block) {
    
}

void bwrite(int block_num, unsigned char *block) {
    lseek(block, block_num * BLOCK_SIZE, SEEK_SET);
    
}

