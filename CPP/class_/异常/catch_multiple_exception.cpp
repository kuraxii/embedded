#include <iostream>
using namespace std;

class Base
{

  public:
    Base()
    {
    }
    ~Base()
    {
    }
    virtual void what()
    {
        cout << "Base exception" << endl;
    }
};

class Derived : public Base
{

  public:
    Derived()
    {
    }
    ~Derived()
    {
    }
    void what()
    {
        cout << "Derived exception" << endl;
    }
};

void func1(int i)
{
    if (i == 0)
    {
        int aa = 100;
        throw aa;
    }
    else if (i == 1)
    {
        throw "this is an exception";
    }
    else if (i == 2)
    {
        Derived d1;
        throw d1;
    }
}

int main(int argc, char *argv[])
{
    try
    {
        func1(2);
    }
    catch (int &e)
    {
        cout << "e= " << e << endl;
    }
    catch (const char* str)
    {
        cout << "e= " << str << endl;
    }
    catch (Base& e)
    {
        e.what();
    }
    return 0;
}