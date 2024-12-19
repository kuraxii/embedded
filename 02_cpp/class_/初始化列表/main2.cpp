#include <iostream>
using namespace std;

struct Person{
    int m_age;
    int m_height;

    Person() : Person(0, 0)
    {
        
    }

    Person(int age, int height)
    {
        m_age = age;
        m_height = height;
    }

};

int main()
{
    Person person;

    cout << person.m_age << endl;
    cout << person.m_height << endl;





    return 0;
}