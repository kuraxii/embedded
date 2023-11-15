#include <iostream>
#include <exception>
using namespace std;

int main(int argc, char *argv[])
{
    string s1 = "this is a big world";

    try{
        cout << "try 1" << endl;
        char c1 = s1[100];
        
        cout << "c1 = " << c1 << endl;
    }
    catch(exception& e)
    {
        cout << "exception 1 catched" << endl;
    }

    try{
        cout << "try 1" << endl;
        char c2 = s1.at(100);
        cout << "c1 = " << c2 << endl;
    }
    catch(exception& e)
    {
        cout << "exception 2 catched" << endl;
        cout << e.what() << endl;
    }
    
 
    return 0;
}