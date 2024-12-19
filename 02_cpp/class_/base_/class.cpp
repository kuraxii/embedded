#include <iostream>
using namespace std;

// class Person
// {
//     int age;
//     void run()
//     {
//         cout << "Person::run-" << age << endl;
//     }
// };

struct Person
{
    int age;
    void run()
    {
        cout << "Person::run-" << age << endl;
    }
};

int main()
{
    Person person;
    person.age = 16;
    person.run();

    return 0;
}