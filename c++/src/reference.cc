#include <cstdio>

int main()
{
    int b = 4;
    
    int *ptr = &b;

    int * const & ref = ptr;

    ref++;

    printf("ref: 0x%llx, ptr = 0x%llx, *ptr = %d\n", ref, ptr, *ptr);
}
