/*********************************************
* @FileName: test.cpp
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @Created Time: Wed Jul 19 13:50:25 2023
*********************************************/

#include<iostream>
#include <malloc.h>
using namespace std;

int main(int argc, char *argv[])
{
    int a;
    int *p = (int *)malloc(1);
    cout << "&a" << &a << endl;
    cout << "p" << p << endl;
	return 0;
}
