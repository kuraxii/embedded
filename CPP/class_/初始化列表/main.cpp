#include <iostream>
#include <type_traits>
using namespace std;

struct Person
{
    int m_height;
    int m_age;

    // Person(int age, int height)
    // {
    //     m_age = age;
    //     m_height = height;
    // }

    Person(int age = 0, int height = 0) : m_age(m_height), m_height(height)
    {
    }
};



int main()
{
    Person person;
    cout << person.m_age << endl;
    cout << person.m_height << endl;
    

    return 0;
}
