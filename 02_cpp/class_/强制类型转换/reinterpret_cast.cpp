#include <iostream>
using namespace std;

class Demo 
{
public:
    int d = 22;
    int e = 33;
    
    void show()
    {
        cout << d << ", "<< &d <<  endl;
        cout << e << ", " << &e << endl;
    }
};


int main(int argc, char *argv[])
{
    
    // long l1 = 0x0000363534145154;
    // cout << "l1= " << l1 << endl;
    // char *pl = reinterpret_cast<char*>(&l1);
    // cout << "pl= " << pl << endl;

    int cc = 400;
    int dd = 500;

    cout << cc << ", " << &cc << endl;
    cout << dd << ", " << &dd << endl;
    

    // Demo d2;

    Demo *pD = reinterpret_cast<Demo*>(&dd);
    pD->show();
    return 0;

}