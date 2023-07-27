#include <iostream>
using namespace std;

class Person
{
    int m_age;

public:
    Person(int m_age = 0) : m_age(m_age){}
    Person(const Person &p) : m_age(p.m_age){}
};

class Student : Person
{
    int m_score;
public:
    Student(int m_age, int m_score) : Person(m_age), m_score(m_score){}
    Student(const Student& stu) : Person(stu), m_score(stu.m_score){}

};


int main(int argc, char *argv[])
{
    


    return 0;
}