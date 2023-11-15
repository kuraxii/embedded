#include <iostream>

class Person
{
  public:
    int m_age;
    void run()
    {
    }
  private:
    int high;

};

class Student : public Person  // 以public的方式继承,私有成员无法被继承
{
  public:  
    void study()
    {
        // high = 10;
    }
};

class Goodstudent : Student
{
    public:
        void work()
        {
            m_age = 10;
        }


};

int main()
{
    Person person;

    Student stu;


    return 0;
}