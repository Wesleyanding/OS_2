#include <stdio.h>
#include <unistd.h>
#include "image.h"
#include "block.h"

int main() {
    image_open("testfs", 1);
    image_close();

    return 0;
}