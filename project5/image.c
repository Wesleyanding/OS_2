#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int image_fd;

int image_open(char *filename, int truncate)
{
    if (truncate)
    {
        image_fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0600);
    }
    else
    {
        image_fd = open(filename, O_RDWR);
    }
    if (image_fd < 0)
    {
        perror("open");
        return -1;
    }
    return image_fd;
}

int image_close(void)
{
    int result = close(image_fd);
    if (result < 0)
    {
        perror("close");
    }
    return result;
}