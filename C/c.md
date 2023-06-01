# C语言查缺补漏

## 基础

> 赋值运算符： 赋值运算符的返回值为赋值本身（从右至左运算）

```c
//example
int a = 100;
int x = a = 90;
//结果为 x = 90，其中x的值为a = 90,为这个运算符的返回值
//在系统编程中的案例
if((len = read(fd,buf,sizeof(buf))) != 0)
// 此时比较的是 0 与 len = read(fd,buf,sizeof(buf))这个赋值运算符的返回值
```

> 逗号表达式： 逗号表达式的值为最后一个表达式的值（从左至右运行）

```c
//example
void func()
{
    int a = 10;
    int b = 10, c, d, e;
    int r = (b++, c++, d + 3, e = b + c, ++a);
    printf("a=%d,b=%d,c=%d,d=%d,e=%d,r=%d\n",a,b,c,d,e,f,r);
}
// 其中r为最后一个表达式++a的值

void f3()
{
    int a = 10;
    int b = 20;
    int r = (a++, b++);
    printf("a=%d,b=%d,r=%d", a, b, r);
}
// 其中r的值为b，先赋值再执行++操作
```

> 移位运算符

右移 >>
(1) 无符号右移 补0
(2) 有符号右移 补符号位

左移 <<
均补0

```c
// example
char i = 127; // 0111 1111
char j = -1;  // 1111 1111
unsigned char k = 255; // 1111 1111

//左移 补0
i << 1;  // 0111 1111 -> 1111 1110
j << 1;  // 1111 1111 -> 1111 1110
k << 1;  // 1111 1111 -> 1111 1110

//右移  补符号位
i >> 1;  // 0111 1111 -> 0011 1111
j >> 1;  // 1111 1111 -> 1111 1111
k >> 1;  // 1111 1111 -> 0111 1111 
```

> register 和 auto

- register和auto都是只能声明局部变量，不能声明全局变量
- 局部变量存储在栈中
- 使用register定义的变量，没有内存地址，无法对其进行取地址操作
```c
// example
int i; // ==> auto int i;   使用auto时，可以省略auto
register int j; // register不能省略，使用该关键字的变量存储一般在寄存器
int *p = &j; //将会报错 error: address of register variable ‘j’ requested
```

> 多文件编译

extern关键字，.h文件, 与条件编译
[详细解析](https://www.runoob.com/w3cnote/extern-head-h-different.html)

函数的声明可以不用加extern

```c
// 全局变量的声明与定义
extern int a; // 声明一个全局变量 a
int a; // 定义一个全局变量 a
extern int a =0 ; // 定义一个全局变量 a 并给初值。
int a =0;    // 定义一个全局变量 a, 并给初值，
```

example
```c
// main.c
#include <stdio.h>
#include "func.h"
int main(int argc, char *argv[])
{ 
    func1();
    func2();
    printf("%d\n",a);
    return 0;
} 
```

```c
// func.c
#include <stdio.h>
#include "func.h"  // 被调用文件应该导入自己的头文件，以至于无视函数间的顺序

int a = 1;
void func1(){
    printf("this is func1\n");
}
void func2(){
    printf("this is func2\n");
}
```

```c
// func.h
#ifndef FUNC_H  // 条件编译命令在预处理时起作用
#define FUNC_H  

void func1();
void func2();
extern int a;

#endif
```

## 指针

> 指针变量的定义
```c
基本指针变量
\[存储类型\] 数据类型 *变量名


``` 