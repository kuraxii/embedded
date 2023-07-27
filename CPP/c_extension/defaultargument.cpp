#include <iostream>
using namespace std;

int add(int num1, int num2, int num3)
{
    return num1 + num2 + num3;
}

int add0(int num1 = 2, int num2 = 3, int num3 = 0)
{
    return num1 + num2 + num3;
}

int main()
{
    cout << "add=" << add(1, 2, 3) << endl;
    cout << "add0=" << add0() << endl;
    return 0;
}
