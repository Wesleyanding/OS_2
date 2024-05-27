#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "inode.h"
#include "free.h"
#include "block.h"
#include "pack.h"
#include "image.h"
#include "dir.h"

#define BLOCK_SIZE 4096

struct directory *directory_open(int inode_num) {
    //Use iget() to get the inode for this file.
    struct inode *inode = iget(inode_num);

    // If it fails, directory_open() should return NULL.
    if (!inode) return NULL;

    // malloc() space for a new struct directory.
    struct directory *dir = malloc(sizeof(struct directory));

    // In the struct, set the inode pointer to point to the inode returned by iget().
    dir->inode = inode;

    // Initialize offset to 0.
    dir->offset = 0;

    // Return the pointer to the struct.
    return dir;
}

int directory_get(struct directory *dir, struct directory_entry *ent) {
    // If the offset is greater than or equal to the size of the file, return 0.
    if (dir->offset >= dir->inode->size) return -1;

    // Compute the block in the directory we need to read.
    int data_block_index = dir->offset / BLOCK_SIZE;

    // Read the appropriate data block  so we can read the directory entry.
    int data_block_num = dir->inode->block_ptr[data_block_index];

    // Read the data block
    unsigned char block[BLOCK_SIZE];
    bread(data_block_num, block);

    // Compute the offset within the block.
    int offset_in_block = dir->offset % BLOCK_SIZE;

    // extract the directory entry from the raw data in the block into the struct directory_entry.
    ent->inode_num = read_u16(block + offset_in_block);
    strcpy(ent->name, (char *)block + offset_in_block + 2);

    return 0;
}

void directory_close(struct directory *d){
    // Call iput() on the inode pointer in the struct directory.
    iput(d->inode);

    // Free the struct directory.
    free(d);
}
 
void mkfs(void) {

    // Get an inode for that file (ialloc()).
    struct inode *inode = ialloc();
    int inode_num = inode->inode_num;
    // Get a data block to hold the hardcoded entries (alloc()).
    int block_num = alloc();

    // Initialize the inode to have the correct metadata.
    struct inode *root_inode = iget(inode_num);
    root_inode->flags = 2;
    root_inode->size = 64;
    root_inode->block_ptr[0] = block_num;

    // Write the directory entries to an in-memory block.
    unsigned char block[BLOCK_SIZE];
    memset(block, 0, BLOCK_SIZE);

    // Write the "." entry.
    write_u16(block, inode_num);
    strcpy((char *)block + 2, ".");

    // Write the ".." entry.
    write_u16(block, inode_num);
    strcpy((char *)block + 34, "..");

    // Write the block out to disk.
    bwrite(block_num, block);

    // Write the inode out to disk.
    iput(root_inode);
}
