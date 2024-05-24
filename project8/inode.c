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
    int block_num = INODE_FIRST_BLOCK + inode_num / INODES_PER_BLOCK;
    int offset = (inode_num % INODES_PER_BLOCK) * INODE_SIZE;
    unsigned char buf[BLOCK_SIZE];
    bread(block_num, buf);
    
    void *addr = buf + (offset * INODE_SIZE);
    in->inode_num = read_u32(addr);
    in->ref_count = read_u32(addr + sizeof(unsigned int));
    in->size = read_u32(addr + 2 * sizeof(unsigned int));
    in->permissions = read_u32(addr + 3 * sizeof(unsigned int));
    in->flags = read_u32(addr + 4 * sizeof(unsigned int));
    in->link_count = read_u32(addr + 5 * sizeof(unsigned int));
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        in->block_ptr[i] = read_u32(addr + 6 * sizeof(unsigned int) + i * sizeof(unsigned short));
    }
}

void write_inode(struct inode *in) {
    int block_num = INODE_FIRST_BLOCK + in->inode_num / INODES_PER_BLOCK;
    int offset = (in->inode_num % INODES_PER_BLOCK) * INODE_SIZE;

    unsigned char buf[BLOCK_SIZE];
    bread(block_num, buf);

    void *addr = buf + (offset * INODE_SIZE);

    write_u32(addr + 2 * sizeof(unsigned int), in->size);
    write_u32(addr + 3 * sizeof(unsigned int), in->permissions);
    write_u32(addr + 4 * sizeof(unsigned int), in->flags);
    write_u32(addr + 5 * sizeof(unsigned int), in->link_count);
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        write_u32(addr + 6 * sizeof(unsigned int) + i * sizeof(unsigned short), in->block_ptr[i]);
    }

    bwrite(block_num, buf);

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

struct inode *ialloc(void)
{
    unsigned char inode_bitmap[BLOCK_SIZE];
    bread(1, inode_bitmap);
    int inode_num = find_free(inode_bitmap);
    if (inode_num == -1)
    {
        printf("No free inodes\n");
        return NULL;
    }

    int new_inode_num = inode_num;

    struct inode *in = iget(new_inode_num);
    if (in == NULL)
    {
        printf("Error getting inode\n");
        return NULL;
    }
    in->size = 0;
    in->owner_id = 0;
    in->permissions = 0;
    in->flags = 0;
    in->link_count = 0;
    for (int i = 0; i < INODE_PTR_COUNT; i++)
    {
        in->block_ptr[i] = 0;
    }
    in->inode_num = new_inode_num;

    write_inode(in);
    set_free(inode_bitmap, inode_num, 1);
    bwrite(1, inode_bitmap);
    return in;
};