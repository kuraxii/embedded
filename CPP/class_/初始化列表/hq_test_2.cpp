#include <iostream>
using namespace std;

class Demo
{
  public:
    // Demo();
    // Demo(int a, int b);
    ~Demo();
    void set(int aa, float bb)
    {
        this->aa = aa;
        this->bb = bb;
    }
    void show()
    {
        cout << aa << " " << bb << endl;
        // cout << this << endl;
    }

    void func1()
    {
        cout << "Demo::func1()" << endl;

    }

  private:
    int aa;
    float bb;
};
// Demo::Demo() : aa(0), bb(0)
// {
//     cout << "Demo::Demo()" << endl;
// }
// Demo::Demo(int a, int b) : aa(a), bb(b)
// {
//     cout << "Demo::Demo(int a, int b)" << endl;
// }
Demo::~Demo()
{
    cout << "Demo::~Demo()" << endl;
}

int main()
{
    Demo demo;
    demo.set(10, 20.3);
    demo.show();
    cout << &demo << endl;

    Demo *p = nullptr;
    p->func1();
    

    return 0;
}