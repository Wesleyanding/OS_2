#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int BLOCK_SIZE = 4096; // 4KB

unsigned char *bread(int block_num, unsigned char *block)
{
    int fd = open("disk_image", O_RDONLY);

    lseek(fd, block_num * BLOCK_SIZE, SEEK_SET);
    read(fd, block, BLOCK_SIZE);
    close(fd);
    return block;
}

void bwrite(int block_num, unsigned char *block)
{
    int fd = open("disk_image", O_WRONLY);

    lseek(fd, block_num * BLOCK_SIZE, SEEK_SET);
    write(fd, block, BLOCK_SIZE);
    close(fd);
}
