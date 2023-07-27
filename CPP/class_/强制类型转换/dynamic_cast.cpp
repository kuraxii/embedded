#include <iostream>
using namespace std;

class A
{
protected:
    int a;
public:
    A(int a) : a(a){}
    ~A(){}
    virtual void show()
    {
        cout << "a= " << a << endl;
    }
    
};

class B : public A
{
protected:
    int b;
public:
    B(int a, int b) : A(a), b(b){}
    ~B(){}
    void show()
    {
        cout << "a= " << a << ", b= " << b << endl;
    }
    
};

class C : public B
{
protected:
    int c;
public:
    C(int a, int b, int c) : B(a, b), c(c){}
    ~C(){}
    void show()
    {
        cout << "a= " << a << ", b= " << b << ", c= " << c << endl;
    }
};

void debug()
{
    cout << "------------" << endl;
}

int main(int argc, char *argv[])
{
    A a1(1);
    B b1(10, 20);
    C c1(100, 200, 300);
    
    a1.show();
    b1.show();
    c1.show();
    
    A *pAtmp = &a1;
    A *pA = nullptr;
    B *pB = nullptr;
    C *pC = nullptr;

    debug();
    pAtmp = pA;
    B *b = dynamic_cast<B*>(pA);
    if(b == nullptr)
    {
        cout << "NULL" << endl;
    }

    debug();
    pAtmp = &b1;
    A *a = dynamic_cast<A*>(pAtmp);
    if(a == nullptr)
    {
        cout << "NULL" << endl;
    }

    debug();
    C *c = dynamic_cast<C*>(pAtmp);
    if(c == nullptr)
    {
        cout << "NULL" << endl;
    }


    


    return 0;
}