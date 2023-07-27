#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    try  // 用于检查有可能出现异常的代码
    {
        cout << "try 1" << endl;
        // 程序发生错误 抛出异常
        throw("this is a serious problem");
        // 抛出异常后的代码无法执行
        cout << "try 2" << endl;
    }
    // 捕获异常, 根据异常携带的信息处理错误
    catch(const char* str)
    {
        cout << str << endl;
    }
    return 0;
}