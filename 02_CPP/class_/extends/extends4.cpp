// 派生类的析构和构造

#include <iostream>
using namespace std;
class Base
{
protected:
    int aa; 
public:
    Base(int a) : aa(a)
    {
        cout << "Base::Base(int)" << endl;            
    }
    ~Base()
    {
        cout << "Base::~Base()" << endl;            
                
    }    
    void show()
    {
        cout << "aa = " << aa << endl;
    }


};

class Derived : public Base
{
    int bb;
    public:
        Derived(int a, int b) : Base(a), bb(b)
        {
             cout << "Dervied::Derived()" << endl;   
        }

        ~Derived()
        {
             cout << "Dervied::~Derived()" << endl;   
        }
        void display()
        {
            cout <<"aa= " << aa << ", bb= " <<  bb << endl;
        }
};


int main(int argc, char *argv[])
{
    Derived d(150, 250);
    d.show();
    d.display();

    cout << sizeof(d) << endl;
    return 0;
}

