#define CTEST_ENABLE
#include <stdio.h>
#include <unistd.h>
#include "image.h"
#include "block.h"
#include "ctest.h"

int main() {
    int result;
    
    // Test image_open
    CTEST_VERBOSE(1);
    result = image_open("disk_image", 1);
    CTEST_ASSERT(result != -1, "image_open");

    // Test image_close
    result = image_close();
    CTEST_ASSERT(result != -1, "image_close");
    
    CTEST_RESULTS();
    CTEST_EXIT();

}