#include <iostream>
using namespace std;

void debug()
{
    cout << "---------------" << endl;
}

class Point{
    double x;
    double y;
public:
    Point(double a = 0, double b = 0) : x(a), y(b){}

    void set(double a, double b)
    {
        x = a;
        y = b;
    }
    void show()
    {
        cout << "x= " << x << ",y= " << y <<endl;
    }

    friend Point operator--(Point&);
    friend Point operator--(Point&, int);
    friend Point operator+=(Point&, Point&);
    friend Point operator!(Point&);
    friend bool operator==(Point&, Point&);
    
};

Point operator--(Point& a)
{
    a.x--;
    a.y--;
    return a;
}
Point operator--(Point& a,int b)
{
    Point p(a.x--, a.y--);
    return p;
}
Point operator+=(Point& a, Point& b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}
Point operator!(Point& a)
{
    a.x = -a.x;
    a.y = -a.y;
    return a;
}
bool operator==(Point& a, Point& b)
{
    if(a.x != b.x || a.y != b.y)
    {
        return false;
    }
    
    return true; 
}



int main(int argc, char *argv[])
{
    Point p1(5.5, 6.6);
    Point p2(5.5, 6.6);
    Point p3(7.7, 8.8);

    
    (p3--).show();
    p3.show();

    debug();

    (--p3).show();
    p3.show();

    debug();

    p3+=p2;
    p3.show();

    debug();
 
    !p3;
    p3.show();

    cout << "p1 == p2 >> " << (p1 == p2) << endl;


    return 0;
}