#include <iostream>
#include <ostream>
#include <type_traits>
using namespace std;




 
void pointer_change(int *a, int *b)
{
    int tem = *a;
    *a = *b;
    *b = tem;
}
int refer_change(int &a, int &b)
{
    int tem = a;
    a = b;
    b = tem;
    return 0;
}

int refer_change(const int &a, const int &b) // 常引用重载
{
    return a + b;
}

int &retref(int &a)
{

    int &ref = a;
    return ref;
}
int ptr(const int *p)
{
    return *p;
}

// int& test()
// {
// 	int a = 10;
// 	int& b = a;
// 	cout << "test: &a=" << &a <<endl;
// 	cout << "test: &b=" << &b <<endl;
// 	return b;
// }


int main()
{
    // int num = 10;
    // int &ref = num;

    // std::cout<<"ref= "<<ref<<std::endl;
    // std::cout<<"&num= "<<&num<<std::endl;
    // std::cout<<"&ref= "<<&ref<<std::endl;

    // int a = 11;
    // ref = a;
    // std::cout<<"ref= "<<ref<<std::endl;
    // std::cout<<"&num= "<<&num<<std::endl;
    // std::cout<<"&ref= "<<&ref<<std::endl;

    // 引用传参
    // int a = 5;
    // int b = 6;
    // pointer_change(&a, &b);
    // refer_change(a, b);

    // std::cout<<"a= "<<a<<std::endl;
    // std::cout<<"b= "<<b<<std::endl;

    // 数组引用
    // int num = 3;
    // int arr[3] = {0};
    // int (&ref)[sizeof(arr)/sizeof(arr[0])] = arr;
    // 指针形式
    // int arr[] = {1, 2, 3};
    // int *const &ref = arr;

    // 常引用
    // int num = 10;
    // const int &ref = num;
    // num++;
    // ref++;

    // 常引用指向不同类型的变量
    // int num = 6;
    // const double &ref = num;
    // num = 30;

    // std::cout<<"num= "<<num<<std::endl;
    // std::cout<<"ref= "<<ref<<std::endl;
    // std::cout<<"&num= "<<&num<<std::endl;
    // std::cout<<"&ref= "<<&ref<<std::endl;
 	
	
	int a = 10;
	//int &ref = a;
	// cout << "&a  =" << &a << endl;
	// cout << "&ref=" << &ref << endl;
	
    typedef struct {
	    int &ref;	
    } REF;

	REF r = {a};
	
	cout << "&r    =" << &r << endl;
	cout << "&r.ref=" << &r.ref << endl;
	cout << "&a    =" << &a << endl;


	// int &v = test()   ;
	// cout << "main: " << &v <<endl;


    return 0;
}
