#include <cstdio>

int main()
{
    const char *str = "test";

    printf("sizeof(\"test\"): %llu\n", (unsigned long long) sizeof("test"));

    printf("sizeof(str): %llu\n", (unsigned long long) sizeof(str));

    printf("sizeof(*str): %llu\n", (unsigned long long) sizeof(*str));

    return 0;
}
