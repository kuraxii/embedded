/*********************************************
* @FileName: memmanage.cpp
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @Created Time: Wed Jul 19 13:55:05 2023
*********************************************/

#include<iostream>
using namespace std;

struct Car{
    int m_price;
    
    Car()
    {
        cout << "Car::Car()" << endl;
    }

    ~Car()
    {
        cout << "Car::~Car" << endl;
    }

};

struct Person{
    int m_age;
    Car* m_car;
    Person()
    {
        m_age = 0;
        m_car = new Car;
        cout << "Person:: Person()" << endl;
    }

    ~Person()
    {
        delete m_car;
        cout << "Person:: ~Person()" << endl;  
        
    }


};


int main(int argc, char *argv[])
{
    {
        Person person;
    }
	return 0;
}
