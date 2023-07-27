#include <iostream>

void func()
{
    std::cout << "hello namespace: std" << std::endl;
}

namespace myspace
{
    int aa = 10;
    void func()
    {
        std::cout << "hello namespace: myspace" << std::endl;
    }

    struct Demo1{
        int a;
        float b;
        std::string c;
    };

    class Demo
    {
        int a;
        float b;
        std::string c;
    };



    namespace my2space
    {
        int c = 10;
        void func()
        {
            std::cout << "hello my2spacce" << std::endl;
        }


    }

} // namespace myspace

int main()
{
    func();
    ::func();
    myspace::func();
    myspace::my2space::func();
    return 0;
}
