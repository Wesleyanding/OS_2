#define CTEST_ENABLE
#include <stdio.h>
#include <unistd.h>
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

    // block.c tests
    // Test bread

    // Test bwrite

    // Test alloc

    // inode.c tests
    // Test ialloc


    // free.c tests
    // Test set_free

    // Test find_low_clear_bit

    // Test find_free

    

    CTEST_RESULTS();
    CTEST_EXIT();
}