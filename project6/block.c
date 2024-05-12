#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "free.h"

int BLOCK_SIZE = 4096; // 4KB

unsigned char *bread(int block_num, unsigned char *block)
{
    int fd = open("disk_image", O_RDONLY);
    if (fd == -1)
    {
        printf("Error opening disk_image\n");
        return NULL;
    }
    if (lseek(fd, block_num * BLOCK_SIZE, SEEK_SET) == -1) {
        printf("Error seeking disk_image\n");
        close(fd);
        return NULL;
    };
    if (read(fd, block, BLOCK_SIZE) == -1) {
        printf("Error reading disk_image\n");
        close(fd);
        return NULL;
    }
    close(fd);
    return block;
}

void bwrite(int block_num, unsigned char *block)
{
    int fd = open("disk_image", O_WRONLY);
    if (fd == -1)
    {
        printf("Error opening disk_image\n");
        return;
    }
    if (lseek(fd, block_num * BLOCK_SIZE, SEEK_SET) == -1) {
        printf("Error seeking disk_image\n");
        close(fd);
        return;
    }
    if (write(fd, block, BLOCK_SIZE) == -1) {
        printf("Error writing disk_image\n");
        close(fd);
        return;
    }
    close(fd);
}

int alloc(void)
{
    unsigned char block_bitmap[512];
    bread(0, block_bitmap);
    int block_num = find_free(block_bitmap);
    if (block_num == -1)
    {
        printf("No free blocks\n");
        return -1;
    }
    set_free(block_bitmap, block_num, 1);
    bwrite(0, block_bitmap);
    return block_num;
}
