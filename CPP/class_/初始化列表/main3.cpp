#include <iostream>
using namespace std;


class Person{
    int m_age;
    public:
    Person()
    {
        cout << "Person::Person()" << endl;

    }
    Person(int age)
    {
        m_age = age;
    }
    ~Person()
    {
        cout << "Person::~Person()" << endl;
    }
};

class Student : Person{
    int m_no;
public:
    Student(int age) : Person(age)
    {
        cout << "Student::Student()" << endl;

    }
    ~Student()
    {
        cout << "Student::~Student()" << endl;
    }
    
};

int main()
{
    {
        Student studen(15);
    }


    return 0;
}