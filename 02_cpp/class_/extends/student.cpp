#include <condition_variable>
#include <iostream>
using namespace std;
class Person
{
  protected:
    string name;
    int age;

  public:
    Person(string name, int age) : name(name), age(age)
    {
        cout << "Person::Person(string, int)" << endl;
    }
    ~Person()
    {
        cout << "Person::~Person()" << endl;
    }
};

class Student : public Person
{
    float score;

  public:
    Student(string name, int age, float score) : Person(name, age), score(score)
    {
        cout << "Student::Student(string, int, float)" << endl;
    }
    ~Student()
    {
        cout << "Student::~Student()";
    }

    void display()
    {
        cout << "name:" << name << ", age:" << age << ", score:" << score << endl;
    }
};

int main()
{
    Student stu("小红", 16, 99.9);
    stu.display();

    return 0;
}
