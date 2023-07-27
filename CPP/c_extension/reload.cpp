/*********************************************
* @FileName: reload.cpp
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @Created Time: Tue Jul 18 14:05:26 2023
*********************************************/

#include<iostream>
using namespace std;


int add(int a, int b)
{
	return a + b;
}

float add(float a, float b)
{
	return a + b;
}


int main(int argc, char *argv[])
{
	cout << "1 + 2 = " << add(1, 2) << endl;
	cout << "0.1 + 0.2 = " << add((float)0.1, (float)0.2) << endl;
	return 0;
}
