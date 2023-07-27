#include <exception>
#include <iostream>
using namespace std;

class NewException 
{
protected:
    int errCode;
    string errInfo;
public:
    NewException(int code, string errinfo) : errCode(code), errInfo(errinfo){}
    ~NewException(){}
    void show()
    {
        cout << "errCode: " << errCode << ", errInfo: " << errInfo << endl;
    }
};


int _div(int a, int b)
{
    if(b != 0)
    {
        return a / b;
    }else {
        throw NewException(111, "div 0");
    }
}


int main(int argc, char *argv[])
{
    try {
        int c = _div(5, 0);         
    } catch (NewException& e) {
        e.show();
    }




    return 0;
}