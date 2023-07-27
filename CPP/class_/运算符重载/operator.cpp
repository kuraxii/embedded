#include <ios>
#include <iostream>
#include <string>
using namespace std;

class Integer
{
    public: 
        Integer(int a = 100) : aa(a){}
        


        void set(int a)
        {
            aa = a;
        }

        void show()
        {
            cout << "aa= " << aa << endl;
        }

    private:
        int aa;
        friend Integer operator+(const Integer&, const Integer&);
        friend Integer operator*(const Integer&, const Integer&);
        friend Integer operator++(Integer&);
        friend Integer operator++(Integer&, int);
        friend bool operator>(const Integer&, const Integer&);

};

// 友元运算符重载
Integer operator+(const Integer& a, const Integer& b)
{
    Integer tmp(a.aa + b.aa);
    return tmp;
}

Integer operator*(const Integer& a, const Integer& b)
{
    Integer tmp(a.aa * b.aa);
    return tmp;
}

Integer operator++(Integer& a) // 前++
{
    Integer tmp(a.aa += 1);
    return tmp;
}

Integer operator++(Integer& a, int b) // 后++
{
    Integer tmp(a.aa++);
    
    return tmp;
}

bool operator>(const Integer& a, const Integer& b)
{
    if(a.aa > b.aa)
    {
        return true;
    }else {
        return false;
    }
}


// void operator=(Integer& a, Integer& b)
// {

// }




int main(int argc, char *argv[])
{
    // int a = 100, b = 300, c;
    // c = a + b;
    // cout << "c= " << c << endl;

    // string s1 = "big world";
    // string s2 = "small world";
    // string s3 = s1 + " " + s2;
    // cout << s3 << endl;

    Integer i1(10), i2(20), i3;
    i1.show();
    i2.show();
    i3.show();
    
    i3 = i1 + i2;
    i3.show();

    i3 = i1 * i2;
    cout << "i1 * i2 ";
    i3.show();

    cout << "i1++ ";
    i1++;
    i1.show();

    cout << "++i1 ";
    ++i1;
    i1.show();


    i1.set(50);
    cout << (i1 > i2) << endl;



    return 0;
}