#include <iostream>
#include <string>
using namespace std;

class Company
{
protected:
    string name;
public:
    Company(string name) : name(name)
    {}
    ~Company(){}
    virtual void slogan()
    {
        cout << name << ", this is a great Company" << endl;
    }
};

class Nike : public Company
{
    
public:
    Nike(string name) : Company(name){}
    ~Nike(){}
    void slogan()
    {
        cout << name << ", just do it!" << endl;
    }
};

class NikeChina : public Nike
{
    
public:
    NikeChina(string name) : Nike(name){}
    ~NikeChina(){}
    void slogan()
    {
        cout << name << ", Go, Beijing!" << endl;
    }
    void slogan(int i)
    {
        cout << "void slogan(int)" << endl;
    }
};


void saySlogan(Company& p)
{
    p.slogan();
}

int main(int argc, char *argv[])
{
    // Company *com = new NikeChina("耐克中国");
    //com->slogan();
    Company c1("Company");
    Nike n1("Nike");
    NikeChina nc("Nike china");

    c1.slogan();
    n1.slogan();
    nc.slogan();


    saySlogan(c1);
    saySlogan(n1);
    saySlogan(nc);
    
    

    return 0;
}