#include <exception>
#include <iostream>
using namespace std;

class MyException : public exception
{
  private:
    const char *errinfo;

  public:
    MyException(const char *errinfo) : errinfo(errinfo)
    {
    }
    

    virtual const char *what() const noexcept
    {
        return errinfo;
    }
};

double divide(double a, double b)
{
    if (b != 0)
    {
        return a / b;
    }
    else
    {
        MyException e("除数为0, 不被允许");
        
        throw e;
    }
}

int main(int argc, char *argv[])
{
    try
    {
        double a1 = 3.33, a2 = 1.11;
        double a3 = divide(a1, 0);
    }
    catch (exception &e)
    {
        cout << e.what() << endl;
    }

    return 0;
}