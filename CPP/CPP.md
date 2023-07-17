# CPP

## 新增特性



### 带有默认参数的函数

### 函数重载

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
    内联函数具有函数特性，具有语法检查

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



## 面向对象

### 类

关键字 class struct

#### class和struct的区别

class默认访问权限 pravite
struct默认访问权限 public

### this

### 堆空空间申请

### 构造函数

### 析构函数
