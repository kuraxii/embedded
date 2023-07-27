#include <iostream>
using namespace std;


class Base
{
protected:
    int b1;
public:
    Base(int b1) : b1(b1){}
    ~Base(){}

    void show()
    {
        cout << "b1 = " << b1 <<  endl;
    }
    
};

class Derived : public Base
{
protected:
    int d1;
public:
    Derived(int b1 = 0, int d1 = 0) : Base(b1), d1(d1){}
    ~Derived(){}

    void show()
    {
        cout << "b1 = " << b1 << ", d1= " << d1 <<  endl;

    }
    
};


void debug()
{
    cout << "-------------" << endl;
}

int main(int argc, char *argv[])
{
    float aa = 123.2;
    int bb = static_cast<int>(aa);
    cout << bb << endl;

    int cc = 29, dd =20;
    char c1 = static_cast<char>(cc+dd);
    int c2 = (char)cc+dd;

    cout << c1 << endl;
    cout << c2 << endl;
    
    debug();

    Base b1(100);
    Derived d2(1000, 2000);
    b1.show();
    d2.show();

    
    Base *pB = static_cast<Base*>(&d2);
    pB->show();

    // Derived D = static_cast<Derived>(b1);
    debug();

    Derived *pD = static_cast<Derived*>(&b1);
    pD->show();
    
    return 0;
}