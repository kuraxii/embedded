/*********************************************
* @FileName: deep_copy.cpp
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @Created Time: Fri Jul 21 10:43:08 2023
*********************************************/

#include<iostream>
using namespace std;

void debug()
{
    cout << "-------------------" << endl;
}

class Array{
public:
    void show()
    {
        for(int i = 0; i < len; i ++)
        {
            cout << pArray[i] << " ";
        }
        cout << endl;

    }

    int getlen()
    {
        return len;

    }

    void setAll(int num)
    {
        for(int i = 0; i < len; i++)
        {
            pArray[i] = num;
        }
    }

    int& set(int index)
    {
        if(index < len)
        {
            return pArray[index];
        }else
        {
            static int tmp = -1;
            return tmp;
        }
    }




    Array(int len = 100) : len(len), pArray(nullptr)
    {

        cout << "Array::~Array(int)" << endl;
        if(len > 0)
        {
            pArray = new int[len];
        }
    }
    Array(Array& a) // 深拷贝
    {
        cout << "Array::Array(Array&)" << endl;
        len = a.len;
        pArray = new int[len];
        for(int i = 0; i < len; i++)
        {
            pArray[i] = a.pArray[i];
        }
    }
    ~Array()
    {
        cout << "Array::~Array()" << endl;
        if(pArray != nullptr)
        { 
            delete [] pArray;
            pArray = nullptr;
        }
    }

private:
    int len;
    int* pArray;
};



int main(int argc, char *argv[])
{
    Array arr(50);
    arr.setAll(66);
    arr.show();
    
    Array arr2(arr);
    arr2.show();
    return 0;
}
