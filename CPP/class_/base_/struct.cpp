/*********************************************
* @FileName: getset.cpp
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @Created Time: Tue Jul 18 19:49:41 2023
*********************************************/

#include<iostream>
using namespace std;

class Person{
private:
    int m_age;
public:
    void setAge(int age)
    {
        if(age <= 0)
            age = -1;
        m_age =age;
    }
    int getAge()
    {
        return m_age;
    }

    Person()  // 构造函数
    {
        cout << "Person()" << endl;
    }
    Person(int age)  // 构造函数
    {
        m_age = age;
        cout << "Person(int age)" << endl;
    }
};


int main(int argc, char *argv[])
{
    Person person;
    Person person1(10);

    Person *per = new Person;
    Person *per1 = new Person(16);
    


	return 0;
}
