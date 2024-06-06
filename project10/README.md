# Project 10 - Race Conditions

## Race Condition #1 - ialloc()

#### Where the race is: 
The race condition exists in the `ialloc` function.

### Code:
```
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
```

### Race Example:

1. Process A reads the inode bitmap and finds the inode X free. 
2. Process B reads the inode bitmap and also finds the inode X free.
3. Process A allocates inode X, marks it as used in the bitmap, and writes the bitmap to disk.
4. Process B allocates the same inode X, overwrites the bitmap and marks inode X as used again. 
5. Both processes have now allocated the same inode and led to an inconsistent filesystem. 

#### Fix:
We can use a lock to protect the critical section where the inode bitmap is read, and written. 

```
struct inode *ialloc(void)
{
    pthread_mutex_lock(&bitmap_lock)

    unsigned char inode_bitmap[BLOCK_SIZE];
    bread(1, inode_bitmap);
    int inode_num = find_free(inode_bitmap);
    if (inode_num == -1)
    {
        pthread_mutex_unlock(&bitmap_lock)
        printf("No free inodes\n");
        return NULL;
    }

    int new_inode_num = inode_num;

    struct inode *in = iget(new_inode_num);
    if (in == NULL)
    {
        pthread_mutex_unlock(&bitmap_lock)
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

    pthread_mutex_unlock(&bitmap_lock)

    return in;
};
```
## Race Condition #2 - directory_make()

#### Where the race is:
The race condition is in the `directory_make` function.

### Code:
```
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
    struct directory *parent_dir = directory_open(parent->inode_num);
    struct directory_entry ent;
    while (directory_get(parent_dir, &ent) == 0) {
        if (strcmp(ent.name, ".") == 0 || strcmp(ent.name, "..") == 0) {
            continue;
        }
    }
    // Write the block out to disk.
    write_u16(block + parent->size % BLOCK_SIZE, inode->inode_num);
    strcpy((char *)block + parent->size % BLOCK_SIZE + 2, "new_dir");
    bwrite(data_block_num, block);

    // Update the parent directory's size field (should increase by 32)
    parent->size += 32;
    
    write_inode(parent);
    // Release the new directory's inode and the parent directory's inode.
    iput(inode);
    iput(parent);

    return 0;
    
}
```

### Race Example

1. Process A and Process B both call `directory_make` to create new directories in the same parent directory.
2. Process A and B both read the parent directory's size and pointers.
3. Process A writes its new entry to the parent's data block.
4. Process B writes its new entry, potentially overwriting the changes made by Process A.
5. The parent directory's size may not correctly reflect the addition of both new directories. 

#### Fix:
We can use a lock to protect the critical section where the parent directory is updated. 
```
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

    pthread_mutex_lock(&dir_lock)

    // find the block that will contain the new directory entry using the size and block_ptr fields.
    int data_block_index = parent->size / BLOCK_SIZE;
    int data_block_num = parent->block_ptr[data_block_index];

    // Read the data block unless you're creating a new one and add the new directory entry to it.
    // Read the data block
    struct directory *parent_dir = directory_open(parent->inode_num);
    struct directory_entry ent;
    while (directory_get(parent_dir, &ent) == 0) {
        if (strcmp(ent.name, ".") == 0 || strcmp(ent.name, "..") == 0) {
            continue;
        }
    }
    // Write the block out to disk.
    write_u16(block + parent->size % BLOCK_SIZE, inode->inode_num);
    strcpy((char *)block + parent->size % BLOCK_SIZE + 2, "new_dir");
    bwrite(data_block_num, block);

    // Update the parent directory's size field (should increase by 32)
    parent->size += 32;
    
    write_inode(parent);

    pthread_mutex_lock(&dir_lock)

    // Release the new directory's inode and the parent directory's inode.
    iput(inode);
    iput(parent);

    return 0;
    
}
```

## Race Condition #3 - alloc()

#### Where the race is:
The race condition exists in the `alloc` function. 

### Code:
```
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
```
### Race Example:
1. Process A reads the block bitmap and finds block X free.
2. Process B reads the block bitmap and also finds block X free. 
3. Process A allocates block X and marks it as used. 
4. Process B allocates the same block X, overwrites the bitmap, marking it as used again. 
5. Both processes have allocated the same block, leading to and inconsistent filesystem. 

#### Fix:
We can use a lock to protect the critical section where the block bitmap is read, and written. 

```
int alloc(void)
{
    pthread_mutex_lock(&bitmap_lock)

    unsigned char block_bitmap[512];
    bread(0, block_bitmap);
    int block_num = find_free(block_bitmap);
    if (block_num == -1)
    {
        pthread_mutex_unlock(&bitmap_lock)

        printf("No free blocks\n");
        return -1;
    }
    set_free(block_bitmap, block_num, 1);
    bwrite(0, block_bitmap);

    pthread_mutex_unlock(&bitmap_lock)

    return block_num;
}
```

## Race Condition #4 - find_free()

#### Where the race is:
The race conditon exists in the `find_free` function. 

### Code:
```
int find_free(unsigned char *block)
{
    for (int byte = 0; byte < 8; byte++)
    {
        if (block[byte] != 0xFF)
        { // If the byte is not all ones
            int bit = find_low_clear_bit(block[byte]);
            if (bit != -1)
            {
                return byte * 8 + bit;
            }
        }
    }
    return -1;
}
```
### Race Example:
1. Process A and Process B both call `find_free` to find a free block
2. Both processes read the same block as free. 
3. Process A marks the block as used.
4. Process B also marks the block as used. 
5. Both processes have allocated the same block. 

#### Fix:
We can uses a lock around the critical section where the bitmap is read and modified. 

```
int find_free(unsigned char *block)
{
    pthread_mutex_lock(&free_lock)

    for (int byte = 0; byte < 8; byte++)
    {
        if (block[byte] != 0xFF)
        { // If the byte is not all ones
            int bit = find_low_clear_bit(block[byte]);
            if (bit != -1)
            {
                return byte * 8 + bit;
            }
        }
    }
    pthread_mutex_unlock(&free_lock)
    return -1;
}
```
