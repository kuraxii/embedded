#include <iostream>
#include <type_traits>
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
        cout << "A::~A()" << endl;
    }
    
};

class B : public A
{
protected:
    int bb;
public:
    B(int a, int b) : A(a), bb(b)
    {
        cout << "B::B(int)" << endl;
    }
    ~B()
    {
        cout << "B::~B()" << endl;
    }
    
};

class D : public B
{
protected:
    int dd;
public:
    D(int a, int b, int d) :  B(a, b), dd(d)
    {
        cout << "D::D(int)" << endl;
    }
    ~D()
    {
        cout << "D::~D()" << endl;
    }

    void show()
    {
        cout << "aa= " << aa << ", bb= " << bb << ", dd= " << dd << endl;
    }
    
};



int main(int argc, char *argv[])
{
    D d1(1, 10, 100);
    d1.show();

    
    return 0;
}