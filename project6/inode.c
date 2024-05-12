#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "free.h"
#include "block.h"

int ialloc(void)
{
    unsigned char inode_bitmap[512];
    bread(1, inode_bitmap);
    int inode_num = find_free(inode_bitmap);
    if (inode_num == -1)
    {
        printf("No free inodes\n");
        return -1;
    }
    set_free(inode_bitmap, inode_num, 1);
    bwrite(1, inode_bitmap);
    return inode_num;
}
