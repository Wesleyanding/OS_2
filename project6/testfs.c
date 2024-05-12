#define CTEST_ENABLE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "image.h"
#include "block.h"
#include "ctest.h"
#include "free.h"
#include "inode.h"

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

    // Test alloc
    int block_num = alloc();
    CTEST_ASSERT(block_num != -1, "alloc");

    // inode.c tests
    // Test ialloc
    int inode_num = ialloc();
    CTEST_ASSERT(inode_num != -1, "ialloc");

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

    CTEST_RESULTS();
    // CTEST_EXIT();
}