/*********************************************
* @FileName: template.cpp
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @Created Time: Tue Jul 18 14:40:48 2023
*********************************************/

#include<iostream>
using namespace std;

template<typename T>
T add(T a, T b)
{
	return a + b;
}


int main(int argc, char *argv[])
{
	cout << add(10, 20) << endl;
	cout << add(1.1, 2.2) << endl;
	
	string s1 = "hello ";
	string s2 = "world";
	cout << add(s1, s2) << endl;


	return 0;
}
