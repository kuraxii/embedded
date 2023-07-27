/*********************************************
 * @FileName: constobj.cpp
 * @Author: Null-zzj
 * @Mail: zj.zhu.cn@gmail.com
 * @CreatedTime: 周四 7月 20 13:57:16 2023
 * @Descript:
 */
#include <iostream>
using namespace std;

class Demo
{
  public:
    Demo(int a) : aa(a)
    {
        cout << "aa= " << aa << endl;
    }
    // void show()
    // {
    //     cout << "show" << endl;   
    // }
    void show() const 
    {
        cout << "show const" << endl;
    }

  private:
  // const 成员变量，对象一旦生成后，不能改变
    const int aa;
};

int main(int argc, char *argv[])
{
    const Demo d1(100);
    // const 类对象只能调用const的成员函数
    d1.show();

    Demo d(10);
    d.show();
    return 0;
}