/*********************************************
* @FileName: copy.cpp
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @Created Time: Fri Jul 21 09:38:11 2023
*********************************************/

#include<iostream>
using namespace std;

void debug()
{
    cout << "-------------------------" << endl;
}


class Demo{
    public:
        Demo(int a = 0, float b = 0) : aa(a), bb(b)
        {
            cout << "Demo::Demo(int, float)" << endl;
        }
        Demo(const Demo& d)
        {
            cout << "Demo:Demo(const Demo&)" << endl;
            this->aa = d.aa;
            this->bb = d.bb;
        }
        void show(){
            cout << "aa " << aa << endl;
            cout << "bb " << bb << endl;
        }
        void set(int a, float b)
        {
            aa = a;
            bb = b;
        }
    private:
        int aa;
        float bb;
};

void func1(Demo d1)
{
    d1.show();
}

Demo returnDemo()
{
    Demo tmp(500, 400.33);
    tmp.show();
    return tmp;
}

int main(int argc, char *argv[])
{
    Demo d1(100, 1.1);
    d1.show();

    debug();

    Demo d2(d1);
    d2.show();


    debug();
    Demo d3 = d1;
    d3.show();

    debug();
    d1.set(5, 12.1);
    d3 = d1;
    d3.show();
    
    debug();
    d1.set(300, 3.333);
    func1(d1);

    debug();
    Demo d4 = returnDemo();
    d4.show();
    return 0;
}
