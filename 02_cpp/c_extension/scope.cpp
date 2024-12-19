/*********************************************
* @FileName: scope.cpp
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @Created Time: Tue Jul 18 15:45:49 2023
*********************************************/

#include<iostream>
using namespace std;

int aa = 1;

int main(int argc, char *argv[])
{
	int aa = 10;

	cout << "aa=" << aa << endl;
	cout << "::aa=" << ::aa << endl;
	


	return 0;
}
