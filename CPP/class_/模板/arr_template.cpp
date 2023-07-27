#include <iostream>
#include <memory>
#include <ostream>
using namespace std;

template <typename T> class Array
{
    int len;
    T *pArray;

  public:
    Array(int len = 10) : len(len), pArray(nullptr)
    {
        if (len > 0)
        {
            pArray = new T[len];
        }
    }
    ~Array()
    {
        delete[] pArray;
        pArray = nullptr;
    }
    void set(T a)
    {
        for (int i = 0; i < len; i++)
        {
            pArray[i] = a;
        }
    }
    int getLen()
    {
        return len;
    }

    void show();

    T &operator[](int index);
};

template <typename T> void Array<T>::show()
{
    for (int i = 0; i < len; i++)
    {
        cout << pArray[i] << "|";
    }
    cout << endl;
}

template <typename T> T &Array<T>::operator[](int index)
{
    if (index < len)
    {
        return pArray[index];
    }
    else
    {
        static T tmp;
        return tmp;
    }
}


class Point{
    double x;
    double y;
public:
    Point(double x = 0, double y = 0) : x(x), y(y){}
    friend ostream& operator<<(ostream& out, Point &p);
};

ostream& operator<<(ostream& out, Point &p)
{
    out << "x= " <<p.x << ", y= " << p.y;
    return out; 
}

int main(int argc, char *argv[])
{
    // 指定模板类的类型为int
    
    Array<int> intArr;
    intArr.set(100);
    intArr.show();
    for(int i = 0; i < intArr.getLen(); i++)
    {
        intArr[i] = i + 123;
    }
    cout << "---" << endl;
    intArr.show();

    // 指定模板类的类型为Point
    Array<Point> Pointarr(100);
    for(int i = 0; i < Pointarr.getLen(); i++)
    {
        Pointarr[i] = Point(i + 123, i + 654);
    }

    Pointarr.show();
    


    return 0;
}