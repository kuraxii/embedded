#include <iostream>
using namespace std;

class Student
{
  public:
    Student(string n, int a, float s) : name(n), age(a), score(s)
    {
    }

    friend void show_student_info(const Student&);


  private:
    string name;
    int age;
    float score;
};

void show_student_info(const Student& stu)
{
    cout << stu.name << "今年" << stu.age << "岁了，成绩" << stu.score << endl; 
}

int main(int argc, char *argv[])
{
    Student stu("小明", 18, 99.9);
    show_student_info(stu);

    return 0;
}