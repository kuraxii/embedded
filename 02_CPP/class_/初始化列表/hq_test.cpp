#include <iostream>
using namespace std;

class Demo
{
  public:
    Demo();
    Demo(int a, int b);
    ~Demo();
    private:
        int aa;
        int bb;
};
Demo::Demo() : aa(0), bb(0)
{
    cout << "Demo::Demo()" << endl;
}
Demo::Demo(int a, int b) : aa(a), bb(b)
{
    cout << "Demo::Demo(int a, int b)" << endl;
}
Demo::~Demo()
{
    cout << "Demo::~Demo()" << endl;
}

int main()
{

    Demo demo[10];
    Demo *dmo = new Demo(10, 5);
    Demo dem(10, 10);

    return 0;
}