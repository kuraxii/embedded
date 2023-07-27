#ifndef _DECLARE_H
#define _DECLARE_H

class Person{
private:
    int m_age;
public:
    void setAge(int age);
    int getAge();
    Person();
    ~Person();
};

#endif