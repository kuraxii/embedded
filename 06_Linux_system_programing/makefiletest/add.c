#include <stdio.h>

int add(int a, int b)
{
    return a + b + 1;
}
int main()
{

    int a = 5, b = 6;
    printf("%d + %d = %d\n", a, b, add(a, b));
    return 0;
}
