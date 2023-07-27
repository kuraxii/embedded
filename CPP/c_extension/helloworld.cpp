/*********************************************
* @FileName: helloworld.cpp
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @Created Time: Tue Jul 18 10:28:50 2023
*********************************************/
#include <stdlib.h>
#include<iostream>
using namespace std;

int main(int argc, char *argv[])
{
	//cout << "hello world" << endl;
	float f1 = 3.14;
	// c格式的类型转化
	//int a = (int)f1;
	//cout << "int=" << a << endl;

	int b = 48;
	cout << "b=" << b << endl;
	char c1 = (char)b;
	cout << "c1=" << c1 << endl;
	
	// c++ 风格的类型转换
	int	a = static_cast<int>(f1);
	cout << "int=" << a << endl;
	
	char *p = static_cast<char*>(malloc(100));
	




	return 0;
}
