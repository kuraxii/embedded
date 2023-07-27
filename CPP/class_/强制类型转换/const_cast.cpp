#include <iostream>
using namespace std;

void func(const int& a)
{
    cout << "const int& a= " << a << endl;
    int& ref_a = const_cast<int&>(a);
    ref_a++;
    cout << "ref_a= " << ref_a << endl;

}

int main(int argc, char *argv[])
{
    
    int aa = 11;
    cout << "aa = " << aa << endl;
    func(aa);
    cout << "aa = " << aa << endl;




    return 0;
}