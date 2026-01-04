#include "math.h"
#include <stdio.h>
#include <assert.h>

int main()
{
    printf("Test k_min....\n");
    assert(k_min(1, 2) == 1);
    assert(k_min(2, 1) == 1);
    assert(k_min(1, 1) == 1);

    return 0;
}