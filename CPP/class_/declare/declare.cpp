/*********************************************
* @FileName: declare.cpp
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @Created Time: Wed Jul 19 14:28:32 2023
*********************************************/
#include "declare.h"
#include<iostream>


void Person::setAge(int age)
{
    m_age = age;
}

int Person::getAge()
{
    return m_age;
}

Person::Person()
{
    std::cout << "Person::Person()" << std::endl;
}

Person::~Person()
{
    std::cout << "Person::Person()" << std::endl;
}

