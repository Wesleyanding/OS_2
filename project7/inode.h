#ifndef INODE_H
#define INODE_H
#define INODE_PTR_COUNT 16
#define MAX_SYS_OPEN_FILES 64

int ialloc(void);

struct inode {
    unsigned int size;
    unsigned short owner_id;
    unsigned char permissions;
    unsigned char flags;
    unsigned char link_count;
    unsigned short block_ptr[INODE_PTR_COUNT];

    unsigned int ref_count;  // in-core only
    unsigned int inode_num;  // in-core only
};

#endif // !INODE_H