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
};

struct Dog : Animal
{
    void speak()
    {
        cout << "Dog is Speak!" << endl;
    }
    void run()
    {
        cout << "Dog is Run!" << endl;
    }
};

struct Pig : Animal
{
    void speak()
    {
        cout << "Pig is Speak!" << endl;
    }
    void run()
    {
        cout << "Pig is Run!" << endl;
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
    return 0;
}