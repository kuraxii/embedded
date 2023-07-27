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
    

};
class Derived : public Base
{
    
public:
    Derived(){}
    ~Derived(){}
    
    virtual void func1(int a, int b)
    {
        cout << "Derived::func1(int, int), " << a << "," << b << endl;
    }

};

int main(int argc, char *argv[])
{
    Base b1;
    int a = 100, b = 200;
    Derived d1;
    
    b1.func1(a, b);
    d1.func1(a, b);


    return 0;
}