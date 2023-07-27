/*********************************************
* @FileName: main.cpp
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @CreatedTime: 周四 7月 20 13:36:52 2023
* @Descript:
*/

// #include <iostream>
// using namespace std;

int main(int argc, char *argv[])
{
    const int aa = 0;
    // cout << "&aa " << &aa << endl;
    int *a = (int*)&aa;
    *a = 1;

    // cout << "&aa " << &aa << endl;
    // cout << "a " << a << endl;
    
    // cout << "*a " << *a << endl;
    // cout << "aa " << aa << endl;


    // int a = 0;
    // cout << "&a  " << &a << endl;

    // int *p = new int;
    // cout << "*p  " << p << endl;

    // static int c = 0;
    // cout << "&c  " << &c << endl;


    return 0;
}