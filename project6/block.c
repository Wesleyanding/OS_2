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
