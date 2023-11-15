#include <iostream>
using namespace std;

template<typename T>
class Container
{
    T data;
public:
    Container(T a) : data(a){}
    void show();
    void show() const
    {
        cout << data << endl;

    }
    T getData() const;
    T& getData();


};

template<typename T>
void Container<T>::show()
{
    cout << data << endl;
}
template<typename T>
T Container<T>::getData() const
{
    return data;
}

template<typename T>
T& Container<T>::getData() 
{
    return data;
}

int main(int argc, char *argv[])
{
    Container<int> intCon(100);
    intCon.show();

    intCon.getData() = 150;
    intCon.show();

    const Container<string> s2("123456s1");
    
    s2.getData() = "987654";
    s2.show();

    return 0;
}