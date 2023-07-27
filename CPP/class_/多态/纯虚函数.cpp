#include <iostream>
using namespace std;

struct Animal  // 抽象类
{
    virtual void speak() = 0;
    virtual void run() = 0;
};

struct Cat : Animal
{
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
    Animal *anim = new Cat();
    anim->run();

   
    return 0;
}