#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void set_free(unsigned char *block, int num, int set)
{
    int byte = num / 8; // Find the byte
    int bit = num % 8;  // Find the bit
    if (set)
    {
        block[byte] |= (1 << bit);
    }
    else
    {
        block[byte] &= ~(1 << bit);
    }
}

int find_low_clear_bit(unsigned char x)
{
    for (int i = 0; i < 8; i++)
        if (!(x & (1 << i)))
            return i;

    return -1;
}

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