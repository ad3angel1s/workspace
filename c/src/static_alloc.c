#include <stdio.h>

static int a = 4;

int func1();
int func2();

int main()
{
    printf("%d + %d + %d = %d\n", a, func1(), func2(), a+func1()+func2() );
    return 0;
}

int func1()
{
    static int a = 3;
    return a;
}

int func2()
{
    static int a = 2;
    return a;
}
