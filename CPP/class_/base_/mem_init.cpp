/*********************************************
* @FileName: mem_init.cpp
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @Created Time: Wed Jul 19 10:08:46 2023
*********************************************/

#include<iostream>
using namespace std;
struct Person
{
    int age;

     Person()
     {
         cout << "Person()" << endl;
     }


    Person(int age)
    {
        if(age < 0)
        {
            age = -1;
        }
        this->age = age;
        cout << "Person(int age)" << endl;
    }

};

Person g_person;

int main(int argc, char *argv[])
{   
    Person l_person;
    
    Person *p = new Person;
    Person *p1 = new Person();
    Person *p2 = new Person[3];
    Person *p3 = new Person[3]();
    Person *p7 = new Person[3]{};
    
    // Person *p1 = new Person(10);
    // cout << "g_person " << g_person.age << endl; 
    // cout << "l_person " << l_person.age << endl; 
    // cout << "p " << p->age << endl; 
    // cout << "p1 " << p1->age << endl; 

    cout << "p " << p->age << endl;
    cout << "p1 " << p1->age << endl;
    cout << "p2 " << p2->age << endl;
    cout << "p3 " << p3->age << endl;
   
    cout << "p7 " << p7->age << endl;

    
    return 0;
}
