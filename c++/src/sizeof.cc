#include <cstdio>

int main()
{
    char *str = "test";

    printf("sizeof(\"test\"): %llu\n", sizeof("test"));

    printf("sizeof(str): %llu\n", sizeof(str));

    printf("sizeof(*str): %llu\n", sizeof(*str));

    return 0;
}