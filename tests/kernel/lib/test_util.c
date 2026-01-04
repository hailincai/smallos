#include "string.h"
#include "util.h"
#include <stdio.h>
#include <assert.h>

int main()
{
    printf("Test int_to_ascii...\n");
    //-2,147,483,648 到 2,147,483,647
    char target[12];
    int_to_ascii(1, target);
    assert(k_str_cmp("1", target) == 0);
    int_to_ascii(10, target);
    assert(k_str_cmp("10", target) == 0);
    int_to_ascii(-10, target);
    assert(k_str_cmp("-10", target) == 0); 
    int_to_ascii(2147483647, target);
    assert(k_str_cmp("2147483647", target) == 0);    
    int_to_ascii(-2147483648, target);   
    assert(k_str_cmp("-2147483648", target) == 0);

    printf("Test hex_to_ascii...\n");
    hex_to_ascii(15, target);
    assert(k_str_cmp("0X0000000F", target) == 0);
    hex_to_ascii(16, target);
    assert(k_str_cmp("0X00000010", target) == 0);
    hex_to_ascii(2147483647, target);
    assert(k_str_cmp("0X7FFFFFFF", target) == 0);
    hex_to_ascii(-2147483648, target);
    assert(k_str_cmp("0X80000000", target) == 0);    
}