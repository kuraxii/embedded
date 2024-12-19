#include <iostream>
using namespace std;
class Base
{
    
    public: 
        int aa;
        void show()
        {
            cout << "aa= " << aa << endl;
        }
};

class Derived : public Base
{
    public:
        int aa;  // 派生类覆盖了父类的同名成员
        void display()
        {
            cout << "aa= " << aa << endl;
        }

};


int main(int argc, char *argv[])
{
    Derived d;
    d.aa = 15;
    d.display();   
    d.show();

    d.Base::aa = 1000;  // 可以使用子类的父类的类名来访问父类
    d.show();

    return 0;
}
