/*********************************************
* @FileName: memory.cpp
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @Created Time: Tue Jul 18 16:11:41 2023
*********************************************/

#include<iostream>
using namespace std;

int main(int argc, char *argv[])
{
	int *p = new int;
	*p = 1000;

	int *q = new int[10];
	for(int i = 0; i < 10; i++)
	{
		q[i] = i;
	}

	for(int i = 0; i < 10; i++)
	{
		cout << "q[" << i << "]= " << q[i] << endl;
	}


	delete [] q;
	q = nullptr;




	return 0;
}
