#include <iostream>
using namespace std;

struct Animal
{
    int m_age;
    virtual void speak()
    {
        cout << "Animal is Speak!" << endl;
    }
    virtual void run()
    {
        cout << "Animal is Run!" << endl;
    }
    Animal(){
        cout << "Animal::Animal()" << endl; 
    }
    virtual ~Animal(){
        cout << "Animal::~Animal()" << endl; 
    }
};

struct Cat : Animal
{
    int mylife;
    void speak()
    {
        cout << "Cat is Speak!" << endl;
    }
    void run()
    {
        cout << "Cat is Run!" << endl;
    }
    Cat(){
        cout << "Cat::Cat()" << endl; 
    }
    ~Cat(){
        cout << "Cat::~Cat()" << endl; 
    }
};



void liu(Animal *p)
{
    p->run();
    p->speak();
}

int main()
{
    Animal *cat = new Cat();
    cat->m_age = 20;
    cat->speak();  
    cat->run();
    delete cat;
    return 0;
}