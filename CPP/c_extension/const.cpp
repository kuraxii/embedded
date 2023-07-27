#include <iostream>

int main()
{
    const int a = 0;

    int *p = (int *)&a;

    *p = 10;

    std::cout << "&a=" << &a << std::endl;
    std::cout << "p =" << p << std::endl;

    std::cout << "a=" << a << std::endl;
    std::cout << "*p=" << *p << std::endl;

    return 0;
}
