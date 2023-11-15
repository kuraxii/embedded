#include <iostream>
using namespace std;

class Demo
{
  protected:
    int aa;

  public:
    Demo(int a = 10) : aa(a)
    {
        cout << "Demo::Demo(int)" << endl;
    }
    ~Demo()
    {
        cout << "Demo::~Demo()" << endl;
    }
};

class Demo2
{
  protected:
    int aa;

  public:
    Demo2(int a = 100) : aa(a)
    {
        cout << "Demo2::Demo2(int)" << endl;
    }
    ~Demo2()
    {
        cout << "Demo2::~Demo2()" << endl;
    }
};
class A
{
  protected:
    int a;

  public:
    A(int x) : a(x)
    {
        cout << "A::A(int)" << endl;
    }
    ~A()
    {
        cout << "A::~A()" << endl;
    }
};

class B
{
  protected:
    int b;

  public:
    B(int x) : b(x)
    {
        cout << "B::B(int)" << endl;
    }
    ~B()
    {
        cout << "B::~B()" << endl;
    }
};

class C
{
  protected:
    int c;

  public:
    C(int x) : c(x)
    {
        cout << "C::C(int)" << endl;
    }
    ~C()
    {
        cout << "C::~C()" << endl;
    }
};

// 基类的继承顺序在派生类中继承的顺序决定了各基类构造函数调用的顺序， 析构的顺序相反
class Derived : public B, protected C, private A
{
  protected:
    Demo2 demo2;
    Demo demo;
    int d;

  public:
    Derived(int a, int b, int c, int d) : demo(), A(a), B(b), C(c), d(d)
    {
        cout << "Derived::Derived(int, int, int, int)" << endl;
    }
    ~Derived()
    {
        cout << "Derived::~Derived()" << endl;
    }

    void show()
    {
        cout << "a= " << a << " b= " << b << " c= " << c << " d= " << d << endl;
    }
};

int main(int argc, char *argv[])
{

    Derived p1(100, 200, 300, 400);
    p1.show();

    int i = 0;
    return 0;
}