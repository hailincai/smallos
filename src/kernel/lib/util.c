#include "util.h"
#include "types.h"
#include "string.h"

void int_to_ascii(int n, char str[])
{
    /**
     * put n in the increasing order
     */
    int i = 0;
    unsigned int tmp;
    int sign = (n < 0) ? -1 : 1;
    if (sign == -1)
    {
        tmp = -n;
    }else
    {
        tmp = n;
    }

    do {
        str[i] = (tmp % 10) + '0';
        i++;
    } while ((tmp = tmp / 10) > 0);
    if (sign == -1) str[i++] = '-';
    str[i++] = '\0';

    //reverse the string
    int left = 0;
    int right = k_str_len(str) - 1;
    while (left < right) {
        char temp = str[left];
        str[left] = str[right];
        str[right] = temp;
        left++;
        right--;
    }
}

void hex_to_ascii(int n, char str[])
{
    // total will be 10 chars 0XYYYYYYYY
    str[0] = '0';
    str[1] = 'X';
    for (int i = 7; i >= 0; i--) 
    {
        int tmp = n & 0x0f;
        if (tmp < 10) 
        {
            str[i + 2] = tmp + '0';
        }else 
        {
            str[i + 2] = tmp - 10 + 'A';
        }
        n >>= 4;
    }
    str[10] = '\0';
}