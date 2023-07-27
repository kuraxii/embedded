/*********************************************
 * @FileName: main.h
 * @Author: Null-zzj
 * @Mail: zj.zhu.cn@gmail.com
 * @CreatedTime: 周三 7月 19 14:46:30 2023
 * @Descript:
 */

#include <iostream>
#include <malloc.h>
using namespace std;

class Person
{
  private:
    int m_age;

  protected:
    int a2;

  public:
    void setAge(int age);

    int getAge();

    Person(); // 构造函数

    Person(int age); // 构造函数

    ~Person();  // 析构函数
};

void Person::setAge(int age)
{
    if (age <= 0)
        age = -1;
    m_age = age;
}
int Person::getAge()
{
    return m_age;
}
Person::Person() // 构造函数
{
    cout << "Person()" << endl;
}
Person::Person(int age) // 构造函数
{
    m_age = age;
    cout << "Person(int age)" << endl;
}
Person::~Person()
{
    cout << "~Person()" << endl;
}

int main(int argc, char *argv[])
{
    Person person;
    //    Person person1(10);

    Person *per = new Person;
    Person *per1 = new Person(16);
    Person *p = (Person *)malloc(sizeof(Person));

    return 0;
}