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

};


int main(int argc, char *argv[])
{
    Person person;
    person.setAge(-10);
    cout << "age= " << person.getAge() << endl;
    

	return 0;
}
