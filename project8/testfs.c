#define CTEST_ENABLE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "pack.h"
#include "image.h"
#include "block.h"
#include "ctest.h"
#include "free.h"
#include "inode.h"
#include "dir.h"

int main()
{
    int result;
    // image.c tests
    // Test image_open
    CTEST_VERBOSE(1);
    result = image_open("disk_image", 1);
    CTEST_ASSERT(result != -1, "image_open");

    // Test image_close
    result = image_close();
    CTEST_ASSERT(result != -1, "image_close");

    // Test alloc
    int block_num = alloc();
    CTEST_ASSERT(block_num != -1, "alloc");

    // inode.c tests
    // Test ialloc
    struct inode *inode = ialloc();
    CTEST_ASSERT(inode->size == 0, "ialloc size is 0");
    CTEST_ASSERT(inode->owner_id == 0, "ialloc owner_id is 0");
    CTEST_ASSERT(inode->permissions == 0, "ialloc permissions is 0");
    CTEST_ASSERT(inode->flags == 0, "ialloc flags is 0");
    CTEST_ASSERT(inode->link_count == 0, "ialloc link_count is 0");
    

    // free.c tests
    // Test set_free
    unsigned char bitmap[4096];
    set_free(bitmap, 0, 1);
    CTEST_ASSERT(bitmap[0] == 1, "set_free");

    // Test find_low_clear_bit
    int low_clear_bit = find_low_clear_bit(0xFF);
    CTEST_ASSERT(low_clear_bit == -1, "find_low_clear_bit for -1 for all ones");

    low_clear_bit = find_low_clear_bit(0x00);
    CTEST_ASSERT(low_clear_bit == 0, "find_low_clear_bit for 0 for all zeros");

    // Test find_free
    unsigned char test_bitmap[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    int free_bit = find_free(test_bitmap);
    CTEST_ASSERT(free_bit == -1, "find_free returns -1 for all ones");

    test_bitmap[0] = 0x00;
    free_bit = find_free(test_bitmap);
    CTEST_ASSERT(free_bit == 0, "find_free returns 0 for first byte with 0");

     // // block.c tests
    // // Test bread
    unsigned char block[4096];
    unsigned char empty_data[4096] = {0};

    bwrite(0, empty_data);
    unsigned char *read_data = bread(0, block);
    CTEST_ASSERT(read_data != NULL, "bread");
    CTEST_ASSERT(strcmp((char *)read_data, "") == 0, "Initial data is empty");

    // Test bwrite
    unsigned char orginal_data[] = "Hello, World!";
    bwrite(0, orginal_data);
    read_data = bread(0, block);
    CTEST_ASSERT(strcmp((char *)read_data, (char *)orginal_data) == 0, "Data is written");

    unsigned char modified_data[] = "Hello, World! Modified!";
    bwrite(0, modified_data);
    read_data = bread(0, block);
    CTEST_ASSERT(strcmp((char *)read_data, (char *)modified_data) == 0, "Data is modified");

    // directory.c tests
    // Test mkfs
    // mkfs();
    // struct inode *root_inode = iget(1);
    // CTEST_ASSERT(root_inode->flags == 2, "root_inode flags is 2");
    // CTEST_ASSERT(root_inode->size == 64, "root_inode size is 64");
    // CTEST_ASSERT(root_inode->block_ptr[0] != 0, "root_inode block_ptr[0] is not 0");

    // Test directory_open
    struct directory *dir = directory_open(1);
    CTEST_ASSERT(dir != NULL, "directory_open");

    // Test directory_get
    // struct directory_entry ent;
    // int get_result = directory_get(dir, &ent);

    // CTEST_ASSERT(get_result == 0, "directory_get");
    // CTEST_ASSERT(ent.inode_num == 1, "ent inode_num is 1");
    // CTEST_ASSERT(strcmp(ent.name, ".") == 0, "ent name is .");

    // Test directory_close
    directory_close(dir);
    CTEST_ASSERT(dir == NULL, "directory_close");

    CTEST_RESULTS();
    // CTEST_EXIT();
}