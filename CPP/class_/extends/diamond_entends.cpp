#include <iostream>
using namespace std;

class A
{
protected:
    int aa;    
public:
    A(int a) : aa(a)
    {
        cout << "A::A(int)" << endl;
    }
    ~A()
    {
        cout << "A::~A()" <<endl;
    }
    
};

class B1 : virtual public A
{
protected:
    int b1;    
public:
    B1(int a, int b) : A(a), b1(b)
    {
        cout << "B1::B1(int, int)" << endl;
    }
    ~B1()
    {
        cout << "B1::~B1()" <<endl;
    }
};

class B2 : virtual public A
{
protected:
    int b2;    
public:
    B2(int a, int b) : A(a), b2(b)
    {
        cout << "B2::B2(int, int)" << endl;
    }
    ~B2()
    {
        cout << "B2::~B2()" <<endl;
    }
};

class D : public B1, public B2 
{
protected:
    int d;    
public:
    // 在虚继承中，在最低成的派生类除了需要构造直接基类，还需要构造虚基类 
    D(int a, int b1, int b2, int d) : A(a), B1(5, b1), B2(6, b2), d(d)
    {
        cout << "D::D(int, int, int, int)" << endl;
    }
    ~D()
    {
        cout << "D::~D()" << endl;
    }
    void show()
    {
        cout << "a= " << aa << ", B1::b1= " << b1 << ", B2::b2= " << b2 << ", d= " << d << endl; 
    }
};

int main(int argc, char *argv[])
{
    D d1(1, 10, 100, 1000);
    d1.show();
    return 0;
}