#include <iostream>
#include <ostream>
#include <streambuf>
#include <string.h>
using namespace std;

void debug()
{
    cout << "---------------" << endl;
}

class Array
{
    int len;
    int *pArray;

  public:
    Array(int len) : len(len), pArray(nullptr)
    {
        pArray = new int[len];
        for (int i = 0; i < len; i++)
        {
            pArray[i] = i + 12;
        }
    }
    Array(Array &array)
    {
        if (array.len > 0)
        {
            this->len = array.len;
            this->pArray = new int[array.len];
            memcpy(this->pArray, array.pArray, sizeof(int) * array.len);
        }
    }

    ~Array()
    {
        delete[] pArray;
        pArray = nullptr;
    }

    int &operator[](int index);
    Array operator=(const Array &);

    friend ostream &operator<<(ostream &, const Array &p);
};

int &Array::operator[](int index)
{
    return pArray[index];
}

Array Array::operator=(const Array &array)
{
    if(this == &array)
    {
        return *this;
    }
    if (array.len > 0)
    {
        if (this->pArray != nullptr)
        {
            delete[] this->pArray;
        }
        this->len = array.len;
        this->pArray = new int[array.len];
        memcpy(this->pArray, array.pArray, sizeof(int) * array.len);
    }
    return *this;
}

ostream &operator<<(ostream &output, const Array &p)
{

    for (int i = 0; i < p.len; i++)
    {
        output << "Array[" << i << "] = " << p.pArray[i] << endl;
    }
    return output;
}

int main(int argc, char *argv[])
{
    Array *arr = new Array(10);
    arr[7] = 99;
    cout << arr;

    Array arr2(*arr);
    cout << arr2;

    arr2 = *arr;
    cout << "arr2: " << endl;
    cout << arr2;

    return 0;
}
