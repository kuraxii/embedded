#include <iostream>
using namespace std;
void func2()
{
    cout << "func2 1" << endl;
    int aa = 1000;
    throw(aa);
    cout << "func2 2" << endl;
}


void func1()
{
    cout << "func1 1" << endl;
    func2();
    cout << "func1 2" << endl;
}

int main(int argc, char *argv[])
{
    try {
    {
        cout << "try 1-1" << endl;}
        func1();
        cout << "try 1-2" << endl;
    
    } catch (int &e) {
        cout << "e= " << e << endl;
    }
    
    return 0;
}