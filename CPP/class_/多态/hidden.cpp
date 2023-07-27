#include <iostream>
using namespace std;

class Base
{
    
public:
    Base(){}
    ~Base(){}
    
    virtual void func1(int a, int b)
    {
        cout << "Base::func1(int, int), " << a << "," << b << endl;
    }
    
    void func1(float a, int b)
    {
        cout << "Base::func1(int, int), " << a << "," << b << endl;
    }

};
class Derived : public Base
{
    
public:
    Derived(){}
    ~Derived(){}
    
    int func()
    {
        cout << "Derived::func1()" << endl;
        return 0;
    }

};

int main(int argc, char *argv[])
{
    
    Derived d;

    d.func1(10, 20);

    

    return 0;
}