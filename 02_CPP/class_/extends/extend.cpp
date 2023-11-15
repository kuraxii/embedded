/*********************************************
 * @FileName: extend.cpp
 * @Author: Null-zzj
 * @Mail: zj.zhu.cn@gmail.com
 * @CreatedTime: 周三 7月 19 15:03:54 2023
 * @Descript:
 */

#include <iostream>

struct Person
{
    int m_age;
    void run()
    {
    }
};

struct Student : Person 
{
    int m_score;
    void study()
    {
    }
};

struct goodstudent : Student{
    int money;

};

struct Worker : Person
{
    int m_saray;
    void work()
    {
    }
};

int main()
{
    goodstudent stu;
    std::cout << &stu.m_age << "\n" << &stu.m_score <<  "\n" << &stu.money << std::endl;

    return 0;
}