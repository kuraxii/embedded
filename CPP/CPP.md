# CPP

## 从c到c++

### 强制类型转换

```cpp
static_cast
const_cast
reinterpret_cast
dynamic_cast

float a;
int b = static_cast<int>(a);
```


### 带有默认参数的函数

为函数的形参的形参指定默认的参数

默认参数只能写在声明中

```cpp
void func1(int a, float b = 3.14);
```

默认参数的使用的规则：
- 默认参数必须出现在非默认参数之后
- 一旦出现了一个默认参数，它后面的参数必须是默认参数
- 一个函数可以全是默认参数

```cpp
void func1(int a, int b = 5);
```
使用规则: 
    默认参数必须

### 函数重载

函数名一定相同，参数的个数，参数的顺序一定有某一项不同，返回值不影响函数重载
```cpp
int add(int a, int b) { return a + b;}
float add(float a, float b) { return a + b;}  // 重载
int add(float a, int b) { return a + b;}    // 重载
float add(int a, int b) { return a + b;}  // 不是重载
```

### 内联函数

关键字
```cpp
inline
```

- 内联函数的作用

    告诉编译器将函数直接展开为函数代码（使用递归就不会展开内联函数）

- 特点

    可以减少函数调用的开销

    增大代码的体积（尽量不要内联超过10行的代码）

- 与宏的异同

    都可以减少函数调用的开销

    宏为字符替换，替换在预处理时期完成
    字符替换容易出现意想不到的结果

    内联函数由编译器实现，展开在编译时完成
    内联函数具有函数特性，拥有语法检查

eg:
```cpp
#define sum(x) (x+x)
inline int sum(int x) {return x + x;}

int a = 10; sum(a++);
// 在这里宏会产生与内联函数不同的结果
```

### const

在c++中， 由const修饰的存储结构是不像c语言那样可以通过指针修改的。因为在c++中的const修饰的存储结构可能存放在只读内存区，通过指针修改const修饰的存储结构，其结果是未定义的，不可预知的。

### 引用（reference）

关键字 &

#### 介绍

- 引用相当于是变量的别名
- 对引用做计算，就是对引用所指向的白能量做计算
- 在定义的时候就不许初始化，一旦指向了某个变量，就不可改变
- 可以利用引用初始化另一个引用，相当于某个变量的多个别名
- 不存在【引用的引用，指向引用的指针，引用数组】

存在的价值：比指针安全

引用的本质

- 引用的本质就是指针，知识编译器削弱了呀的功能，所以引用就是弱化了的指针
- 一个引用占用一个指针的的大小

指针与数组的引用声明

```cpp
//指针的引用
int *p;
int* &ref = p;

///数组的引用
int arr[] = {1,2,3};
int (&ref)[3] = arr;

int num = 3;
int arr[3] = {0};
int (&ref)[sizeof(arr)/sizeof(arr[0])] = arr;

// 指针形式
int arr[] = {1,2,3};
int* const &ref = arr;
```

#### 常引用

引用可以被const修饰，这样就无法通过引用修改数据了，可以称为常引用

```cpp
const int &ref;
int & const ref; // 这种写法没有任何实际意义，引用本身就无法修改
```

const引用的特点

1. 常引用可以引用一个立即数(常量， 表达式，函数返回值)
    ```c
    int &ref = 10;  // 报错
    const int &ref = 10;  // 可行
    ```
2. 可以引用不同类型的数据
3. 作为函数参数时
    - 可以接收const和非const实参（非const引用只能接收非const实参）
    - 可以跟非const引用构成重载
    ```cpp
    int sum(int &v1, int &v2)
    {
        return v1 + v2;
    }
    int sum(const int &v1, const int &v2)
    {
        return v1 + v2;
    }
    int main()
    {
        // 非const实参
        int a = 1, b = 2;
        sum(a, b);  // 调用 int sum(int &v1, int &v2)
    
        // const实参
        const int c = 1, d = 2;
        sum(c, d); // 调用 int sum(const int &v1, const int &v2)
        sum(1, 2); // 调用 int sum(const int &v1, const int &v2)
    }
    ```
4. 当常引用指向了不同类型的数据时，会产生临时变量，即引用指向的并不是初始化时的变量
```cpp
int num = 6;
const double &ref = num;
num = 30;
std::cout<<"num= "<<num<<std::endl;
std::cout<<"ref= "<<ref<<std::endl;
std::cout<<"&num= "<<&num<<std::endl;
std::cout<<"&ref= "<<&ref<<std::endl;

// 结果如下：
// num= 30
// ref= 6
// &num= 0x7fffed562764
// &ref= 0x7fffed562768
```

### 作用域与命名空间

1. 局部域
2. 名字空间域
3. 类域

4. 变量的作用域
    - 局部变量的作用域是局部，
    - 普通全局变量的作用域是整个项目（别的文件可以访问）
    - 静态全局变量的作用域是本文件,在别的文件是不可见的

5. 全局函数的作用域
    - 普通全局函数的作用域是整个项目（别的文件可以访问）
    - 静态全局函数的作用域是本文件，在别的文件是不可见的


## 面向对象

### 类

关键字 class struct

#### class和struct的区别

class默认访问权限 private，默认继承权限也是 private
struct默认访问权限 public，默认继承权限也是 public 

#### this 指针

this 指针是一个隐式变量，在调用类函数时，会自动将类的首地址传给this指针，可以在类内部函数中使用

#### 堆空空间申请

new delete

构造函数 structor

构造函数(也叫构造器)， 在对象创建的时候自动调用，一般用于完成对象的初始化工作

特点

1. 函数名与类同名，无返回值，可以有参数，可以重载，可以有多个构造函数
2. 一旦自定义了构造函数，必须选择用一个自定义的构造函数来初始化对象

```cpp
struct Person
{
    int age;

     Person()
     {
         cout << "Person()" << endl;
     }

    Person(int age)
    {
        if(age < 0)
        {
            age = -1;
        }
        this->age = age;
        cout << "Person(int age)" << endl;
    }

};
```


### 析构函数 Destructor

析构函数（也叫析构器），在销毁对象的实惠调用，一般用于完成对象后的清理工作

特点

函数名以~开头，与类同名，无返回值，无参，不可以重载，有且只有一个析构函数

通过malloc分配的对象free的时候不会调用构造和析构函数

构造函数与析构函数要设置为public才能正常调用  




### 继承

#### 成员访问权限

public 公共的任何地方都可以访问（struct默认）

protected 子类内部，当前类内部可以访问

private 私有的，只有当前内部类可以访问（class默认）

子类内部访问父类的权限，是一下2项权限最小的那个
- 成员本身的访问权限
- 上一级父类的继承方式

开发中用的最多的继承方式是public，这样保留父类原来的成员访问权限

**访问权限不影响类的内存布局**

#### 初始化列表

##### 初始化列表的使用
- 一种便捷的初始化成员变量的方式
- 初始化顺序只与成员声明的顺序有关
- 只能用于构造函数
- 如果声明和实现是分离的，则初始化列表应位于实现

```cpp
Person(int age, int height)
{
    m_age = age;
    m_height = height;
}
// 等价于
// 在原始代码的右值能放什么，括号里就可以放什么
// 括号里的内容和参数列表并无绝对关系
Person(int age, int height) :m_age(age), m_height(height){}

```
与函数的默认参数配合

```cpp
Person(int age = 0, int height = 0) : m_age(m_height), m_height(height){}
// 可以当作无参构造函数使用
Person person;
Person person1(10);
Person person1(10, 20);
// 以上写法皆可行
```



##### 构造函数的互相调用

构造函数的相互调用不能在函数体内直接调用，而应该在初始化列表中调用
```cpp
struct Person{
    int m_age;
    int m_height;

    Person() : Person(0, 0)  // 调用其他的构造函数
    {  
    }

    Person(int age, int height)
    {
        m_age = age;
        m_height = height;
    }

};
```

父类的构造函数

- 子类的构造函数在执行自己的代码之前默认会调用父类的无参构造
- 对于析构函数，子类会在执行完自己的代码后调用父类的额析构函数
- 如果子类的构造函数显示的调用了父类的有参构造函数，就不会再去默认调用父类的无参构造函数
- 如果父类缺少无参构造函数，子类的 构造函数必须显示的调用父类的构造函数

显示调用
```cpp
class Person{
    int m_age;
    public:
    Person()
    {
        cout << "Person::Person()" << endl;

    }
    Person(int age)
    {
        m_age = age;
    }
};

class Student : Person{
    int m_no;
public:
    Student(int age) : Person(age)
    {
        cout << "Student::Student()" << endl;

    }
};
```

#### 静态成员变量


#### const类与成员

#### 友元

当一个函数或者类想访问另外一个类的私有成员的时候，要先成为它的友元

需要注意的地方：
- 友元关系不能继承
- 友元关系是单向的，如果A是B的友元，但B不一定是A的友元，要看有没有友元的申明
- 友元关系不具有传递性，如果B是A的友元，而C是B的友元，那C不一定是A的友元，要看C是否被定义为A的友		
- 友元它破坏了类的封装和隐藏，所以我们要慎用

三种友元
-   友元函数：
    ```cpp
    class Student
    {
    friend void checkId(Student&);
    }
    // 友元函数可以放在共有区域，也能够放到私有区域
    // 一个函数可以是多个类的友元函数，只需要在各个类中分别声明
    // 友元函数的调用和普通函数一样
    ``` 
    
将一个全局函数声明为类的友元函数，这样的话，这个友元函数就能够访问类对象的私有的成员

友元函数没有类对象的this指针，必须将类对象的指针或者引用传给友元函数

    
- 友元类
    ```cpp
    friend class Demo;
    //可以声明一个类为另一类的友元类，
    ```
- 友员成员函数
    ```cpp
    class B;
    class A{
    public:
        void func(B&);
    
    }
    class B{
    private:
        int a;
        friend void A::func(B&);
    }
    //可以将A的成员函数func设置为B的友元，这个函数就成为了B的友元成员函数
    ```

#### 多继承



### 多态

父类指针与子类指针

父类指针可以指向子类对象，是安全的，开发中经常用到（继承方式必须是public）

多态是面向对象非常重要的一个特性
- 同一个操作作用于不同的对象，可以有不同的解释，产生不同的执行结果
- 在运行时，可以识别出真正的对象类型，调用对应子类中的函数

多态的要素
- 子类重写父类的成员函数（override）
- 父类指针指向子类对象
- 利用父类指针调用重写的成员函数

#### 虚函数

关键字 virtual
c++中的虚函数通过虚函数(virtual function)来实现

- 虚函数: 被virtual修饰的成员函数
- 只要被父类中声明为虚函数，子类重写的函数也会自动变成虚函数（也就是说子类中可以省略）
```cpp
#include <iostream>
using namespace std;

struct Animal
{
    virtual void speak()
    {
        cout << "Animal is Speak!" << endl;
    }
    virtual void run()
    {
        cout << "Animal is Run!" << endl;
    }
};

struct Cat : Animal
{
    void speak()
    {
        cout << "Cat is Speak!" << endl;
    }
    void run()
    {
        cout << "Cat is Run!" << endl;
    }
};

struct Dog : Animal
{
    void speak()
    {
        cout << "Dog is Speak!" << endl;
    }
    void run()
    {
        cout << "Dog is Run!" << endl;
    }
};

struct Pig : Animal
{
    void speak()
    {
        cout << "Pig is Speak!" << endl;
    }
    void run()
    {
        cout << "Pig is Run!" << endl;
    }
};


```

#### 虚表

虚函数的实现原理是虚表，这个虚表里面存储着最终要调用的虚函数地址，这个虚表也叫虚函数表











