#include <iostream>
using namespace std;

class Base
{
protected:
    int bb;
public:
    Base(int b) : bb(b)
    {
        cout << "Base::Base(int)" << endl;
    }
    ~Base(){}
    
    void show()
    {
        cout << "Base::show(), bb= " << bb << endl;
    }
};

class Derived : public Base
{
protected:
    int dd;

public:
    Derived(int bb, int dd) : Base(bb), dd(dd)
    {

    }

    ~Derived()
    {

    }
    
    void show()
    {
        cout << "Derived::show(), bb=" << bb << ", dd= " << dd << endl; 
    }

        
};

int main(int argc, char *argv[])
{
    Base b1(1);
    Derived d1(10, 20);
    b1.show();
    d1.show();

    Base b2(2);
    Derived d2(20, 40);
    b2 = d1;
    b2.show();

    Base& b_ref = d1;
    b_ref.show();
    
    Derived *d;
    d = (Derived*)&b1;
    d->show();





    return 0;
}