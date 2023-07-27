#include <iostream>
using namespace std;
class Demo{
    public:
        static int shared_value;    
        Demo(int a, int b) : aa(a), bb(b)
        {

        }
        void show()
        {
            cout << "aa= " << aa << ",bb= " << bb << endl;
        }
        static void set_value(int value)
        {
            Demo::shared_value = value;
        }
        // 静态成员函数没有this指针，不能访问普通的成员函数
        static void show_value()
        {
            cout << "shared value= " << Demo::shared_value << endl; 
        }

    private:
        int aa;
        float bb;
        // 类的静态成员，可以看称这个类的一个全局变量，对于某一个特定的类，他只有一份
        // 所有的类成员共享这个类成员
        
          // 声明
};

 // 定义
int Demo::shared_value = 0; 

int main()
{
    Demo demo(10, 20);
    cout << "sizeof(demo)" << sizeof(Demo) << endl; 

    demo.show();
    demo.set_value(99);
    demo.show_value();

    int i = 0;
    cout << "demo.shared_value " << &demo.shared_value << endl;
    cout << "i " << &i << endl;

    cout << "sizeof()" << sizeof(Demo) << endl;

    return 0;
}