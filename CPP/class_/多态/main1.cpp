#include <iostream>
using namespace std;

struct Animal
{
    void speak()
    {
        cout << "Animal is Speak!" << endl;
    }
    void run()
    {
        cout << "Animal is Run!" << endl;
    }

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
    // Animal *pig = new Pig;
    // Animal *cat = new Cat;
    // Animal *dog = new Dog;

    // pig->run();
    // pig->speak();

    // cat->run();
    // cat->speak();
    
    // dog->run();
    // dog->speak();

    liu(new Dog());
    liu(new Pig());
    liu(new Cat());

    int mian    ;
    return 0;
}