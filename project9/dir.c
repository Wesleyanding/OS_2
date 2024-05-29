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


struct inode *namei(char *path) {
    // If the path is "/", return the root inode.
    if (strcmp(path, "/") == 0) {
        return iget(1);
    }

    // Otherwise, return NULL.
    return NULL;
}

int directory_make(char *path) {
    // Call namei() to get the inode for the parent directory.
    struct inode *parent = namei(path);

    // If the parent directory does not exist, return -1.
    if (!parent) return -1;

    // Get an inode for the new directory (ialloc()).
    struct inode *inode = ialloc();

    // Get a data block to hold the directory entries (alloc()).
    int block_num = alloc();

    // Create a new block sized array for the new directory data block.
    unsigned char block[BLOCK_SIZE];

    // Init it . and .. files. . should contain the inode number of the new directory and .. should contain the inode number of the parent directory.
    write_u16(block, inode->inode_num);
    strcpy((char *)block + 2, ".");
    write_u16(block + 32, parent->inode_num);
    strcpy((char *)block + 34, "..");

    // initialize the inode to have the correct metadata.
    inode->flags = 2;
    inode->size = 64;
    inode->block_ptr[0] = block_num;

    // Write the block out to disk.
    bwrite(block_num, block);

    // find the block that will contain the new directory entry using the size and block_ptr fields.
    int data_block_index = parent->size / BLOCK_SIZE;
    int data_block_num = parent->block_ptr[data_block_index];

    // Read the data block unless you're creating a new one and add the new directory entry to it.
    // Read the data block
    unsigned char parent_block[BLOCK_SIZE];
    bread(data_block_num, parent_block);

    // Write the block out to disk.
    write_u16(parent_block + parent->size % BLOCK_SIZE, inode->inode_num);

    // Update the parent directory's size field (should increase by 32)
    parent->size += 32;
    
    // Release the new directory's inode and the parent directory's inode.
    iput(inode);
    iput(parent);
    
}
