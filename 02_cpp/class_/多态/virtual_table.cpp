#include <iostream>
using namespace std;
class A
{
  public:
    virtual void func1()
    {
        cout << "class a" << endl;
    }
};
class B : public A
{
  public:
    void func1()
    {
        cout << "class b" << endl;
    }
};

int main(int argc, char *argv[])
{
    A a;
    cout << "sizeof(A) " << sizeof(A) << endl;

    cout << "sizeof(B) " << sizeof(B) << endl;

    void (*p)() = reinterpret_cast<void(*)()>(*reinterpret_cast<int*>(*reinterpret_cast<int*>(&a)));
    p();
    
    return 0;
}
