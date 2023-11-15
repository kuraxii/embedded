#include <iostream>
using namespace std;



class Base
{
  public:
    int a1;
    void func1()
    {
        cout << "Base::func1(), a1= " << a1 << endl;
    }
    void set(int aa, int bb, int cc)
    {
        a1 = aa;
        a2 = bb;
        a3 = cc;
    }

    

  protected:
    int a2;
    void func2()
    {
        cout << "Base::func2(), a2" << a2 << endl;
    }

  private:
    int a3;
    void fun3()
    {
        cout << "Base::func3(), a3" << a3 << endl;
    }
};

class Derived : protected Base
{
    int d1;

  public:
  void func()
    {
        func1();
        func2();
    }
    void show()
    {
        cout << "Derived::show(), d1 = " << d1 << endl;
    }
    void setValue(int a)
    {
        a1 = a;
        a2 = a;
        // a3 = a;
        
        d1 = a;
        set(a + 100, a + 200, a + 300);
    }
};

int main(int argc, char *argv[])
{
    Derived d;
    // d.set(100, 200, 300);

    d.func();
    d.show();

    d.setValue(1000);
    d.show();
    return 0;
}