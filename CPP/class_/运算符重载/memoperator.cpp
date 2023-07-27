#include <iostream>
#include <ostream>
using namespace std;

void debug()
{
    cout << "---------------" << endl;
}

class Point
{
    double x;
    double y;

  public:
    Point(double x, double y) : x(x), y(y)
    {
    }
    void set(int a, int b)
    {
        this->x = a;
        this->y = b;
    }
    void show()
    {
        cout << "x= " << x << ",y= " << y << endl;
    }

    Point operator-=(Point a);
    Point operator++();
    Point operator++(int);
    Point operator!() {
      this->x = -this->x;
      this->y = -this->y;
      return *this;
    }

    friend ostream &operator<<(ostream &, const Point &p);
};

Point Point::operator-=(Point a)
{
    this->x -= a.x;
    this->y -= a.y;
    return *this;
}
Point Point::operator++()
{
    this->x++;
    this->y++;
    return *this;
}
Point Point::operator++(int i)
{
    Point tmp(this->x++, this->y++);
    return tmp;
}

ostream &operator<<(ostream &output, const Point &p)
{

    output << "x: " << p.x << ",y: " << p.y;
    return output;
}

int main(int argc, char *argv[])
{
    Point p1(5, 6);
    Point p2(15, 16);

    (p1++).show();
    cout << "p1" << p1 << endl;
    debug();

    (++p1).show();
    cout << "p1" << p1 << endl;
    debug();


    p1 -= p2;
    cout << "p1" << p1 << endl;
    cout << "p2" << p2 << endl;
    debug();

    !p1;
    cout << "p1" << p1 << endl;
    cout << "p2" << p2 << endl;

    return 0;
}
