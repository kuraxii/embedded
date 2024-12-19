#include <iostream>
#include <string>
using namespace std;

class Phone
{
protected:
    

public:
    
    Phone(){}
    ~Phone(){}
    void call(string str)
    {
        cout << "Make a call to " << str << endl;
    }
};

class Car
{
    string ownerName;
    Phone phone;
public:
    
    Car(string name ) : ownerName(name)
    {

    }
    ~Car(){}

    void driver()
    {
        cout << ownerName << " is driving the car" << endl;
    }
    Phone& getPhone()
    {
        return phone;
    }
    
};


int main(int argc, char *argv[])
{



    return 0;
}