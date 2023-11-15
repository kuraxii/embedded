#include <iostream>
#define MULTIPLY(a,b) (a,b)

using namespace std;

inline int mulitply(int a, int b)
{
	return a * b;
}

inline void func()
{
    cout << "func1" << endl;
    cout << "func2" << endl;
    cout << "func3" << endl;
}

int main()
{
    func();

    return 0;
}
