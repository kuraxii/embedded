/*********************************************
 * @FileName: ioformat.cpp
 * @Author: Null-zzj
 * @Mail: zj.zhu.cn@gmail.com
 * @Created Time: Wed Jul 19 11:05:35 2023
 *********************************************/

#include <iostream>
using namespace std;

void debug(string s1 = "------------------------------")
{
    cout << s1 << endl;
}

int main(int argc, char *argv[])
{
    int aa = 255;
    cout << aa << endl;

    debug();
    cout.setf(ios::showpos);
    cout << aa << endl;

    // 以16进制显示
    debug();
    cout.setf(ios::hex, ios::basefield);
    cout << aa << endl;

    // 以16进制显示，前缀
    debug();
    cout.setf(ios::showbase);
    cout << aa << endl;

    // 设置宽度
    debug();
    cout.width(16); // 设置宽度
    cout.fill('*'); // 设置填充字符
    cout << aa << endl;

    // 左对齐
    debug();
    cout.width(16); // 设置宽度
    cout.fill('*'); // 设置填充字符
    cout.setf(ios::left, ios::adjustfield);
    cout << aa << endl;

    // 右对齐
    debug();
    
    cout.width(16); // 设置宽度
    cout.fill('*'); // 设置填充字符
    cout.setf(ios::right, ios::adjustfield);
    cout << aa << endl;

    // 取消设置
    debug();
    cout.unsetf(ios::showbase);
    cout << aa << endl;

    debug();
    cout.unsetf(ios::basefield);
    cout << aa << endl;
    return 0;
}
