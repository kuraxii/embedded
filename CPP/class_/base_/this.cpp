#include <iostream>
using namespace std;

struct Person{
    int id;
    int age;
    int high;

    void desplay()
    {
        cout << "id= " << id << ", age= " << age << ", high= " << high << endl;
    }

    void desplay2()
    {
        cout << "this= " << this << endl;
        cout << "id= " << this->id << ", age= " << this->age << ", high= " << this->high << endl;
    }

};


int main()
{
    Person person;
    person.id = 16;
    person.age = 30;
    person.high = 180;

    person.desplay();
    person.desplay2();

    cout << "&person " << &person << endl;



}