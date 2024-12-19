#include <atomic>
#include <iostream>
#include <ostream>
#include <string>
using namespace std;

#if 1
template <typename T> // 新模板语法
#else
template <class T> // 老模板语法
#endif

T add(T t1, T t2) // 函数模板
{
    return t1 + t2;
}

template <typename T1>
void exchange(T1& t1, T1& t2)
{
    T1 tmp;
    tmp = t1;
    t1 = t2;
    t2 = tmp;
}


class Point{
    double x;
    double y;
public:
    Point(double x = 0, double y = 0) : x(x), y(y)
    {}

    Point operator+(Point p)
    {
        Point tmp;
        tmp.x = this->x + p.x;
        tmp.y = this->y + p.y;
        return tmp;
    }

    friend ostream& operator<<(ostream& out, const Point& p);

};


ostream& operator<<(ostream& out, const Point& p)
{
    out << "x= " << p.x << ",y= " << p.y << endl;
    return out;
}


int main(int argc, char *argv[])
{

    // cout << add(12, 34) << endl;
    // cout << add(12.6, 34.7) << endl;

    // string a = "hello ";
    // string b = "world";
    // cout << add(a, b) << endl;

    // Point p1(1.1, 2.2), p2(3.3, 4.4), p4(5.5, 6.6);

    // cout <<  p1;
    // cout << add(p1, p2);
    
    int a = 5, b = 6;
    exchange(a, b);
    cout << "a= " << a << ", b= " << b << endl; 
    
    float c = 6.6, d = 7.7;
    cout << "c= " << c << ", d= " << d << endl; 
  


    return 0;
}