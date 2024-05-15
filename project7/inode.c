#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "free.h"
#include "block.h"
#include "inode.h"
#include "pack.h"

#define BLOCK_SIZE 4096
#define INODE_SIZE 64
#define INODE_FIRST_BLOCK 3

#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

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
};

struct inode *incore_find_free(void)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
    {
        if (incore[i].ref_count == 0)
        {
            return &incore[i];
        }
    }
    return NULL;
};

struct inode *incore_find(unsigned int inode_num)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
    {
        if (incore[i].inode_num == inode_num)
        {
            return &incore[i];
        }
    }
    return NULL;
};

void incore_free_all(void)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
    {
        incore[i].ref_count = 0;
    }
};

void read_inode(struct inode *in, int inode_num) {
    
}

void write_inode(struct inode *in) {

}

struct inode *iget(int inode_num) {
    struct inode *in = incore_find(inode_num);
    if (in != NULL) {
        in->ref_count++;
        return in;
    }
    in = incore_find_free();
    if (in == NULL) {
        return NULL;
    }
    in->ref_count = 1;
    in->inode_num = inode_num;
    read_inode(in, inode_num);
    return in;
};

void iput(struct inode *in) {
    if (in->ref_count == 0) {
        return;
    }
    in->ref_count--;
    if (in->ref_count == 0) {
        write_inode(in);
    }
};