#include <exception>
#include <iostream>
#include <stdexcept>
using namespace std;
int g_aa[100];

int func1(int index)
{
    if (index < 100)
        return g_aa[index];
    else
        throw out_of_range("Index is out of range 100");
}
int _div(int a, int b)
{
    if (b != 0)
        return a / b;
    else
        throw invalid_argument("div 0");
}

int main(int argc, char *argv[])
{
    try
    {
        int aa = func1(111);
    }
    catch (exception &e)
    {
        cout << e.what() << endl;
    }

    try
    {
        int bb = _div(6, 0);
    }
    catch (exception &e)
    {
        cout << e.what() << endl;
    }
    return 0;
}