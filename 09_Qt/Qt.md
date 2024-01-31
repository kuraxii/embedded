# QT
## 快捷键
// 运行 CTRL + R  run
// 编译 CTRL + B  build
// 格式化  CTRL + i
// 同名的.cpp .h切换 F4
// 查看文档 F1

## 信号与槽
信号与槽优点，松散耦合

QObject::connect(scrollBar, SIGNAL(valueChanged(int value)),
                   label, SLOT(setNum(int value)));
参数：
    scrollBar：信号发送者   类实例地址
    SIGNAL(valueChanged(int value))：发送的信号  函数指针地址
    label：信号的接收者  类实例地址
    SLOT(setNum(int value)): 槽函数 槽函数地址

**自定义信号与槽的实现**
1. 自定义信号

    自定义信号 写道signals下面
    1. 不需要返回值 void
    2. 只需要声明，不要实现
    3. 可以有参数，可以发生重载
    ```cpp
    signals:
        // 下课函数,定义了一个信号
        void hungry();
    ```
2. 自定义槽函数

    槽函数 写到public slots下，或者 public 或者全局函数 lambda
    1. 返回值 void
    2. 需要声明 需要实现
    3. 可以有参数 可以重载
    ```cpp
    // 声明一个槽函数
    public slots:
        void treat();

    // 定义槽函数
    void student::treat()
    {
        qDebug() << "请老师吃饭";
    }
    ```

3. 发送信号

    使用emit关键字
    ```cpp
    // 发送tea实例的hungry型号
    emit tea->hungry();
    ```

3. 使用connect函数联系起来
    ```cpp
    connect(tea, &teacher::hungry, stu, &stu::treat)
    ```
    
信号与槽发送重载时，使用connect连接时要使用函数指针做类型转化
```cpp
connect(tea, (void(teacher::*)(QString))&teacher::hungry, this->stu, (void(student::*)(QString))&student::treat);
```


信号与槽拓展
1. 信号连接信号 (信号的参数遵循4，5元组)
2. 多个信号连接一个槽函数
3. 一个信号连接多个槽函数
4. 信号和槽的参数类型必须一一对应
5. 信号的参数可以多余槽函数的参数，反之不行 
    1. 当信号与槽函数的参数数量相同时，它们参数类型要完全一致。
    2. 当信号的参数与槽函数的参数数量不同时，只能是信号的参数数量多于槽函数的参数数量，且前面相同数量的参数类型应一致，信号中多余的参数会被忽略。
6. 可以利用disconnect函数断开连接（connect如何传参，dosconnect就怎么传参）

QT4版本的信号槽写法
```cpp
// 优点：参数直观，写法简单
// 缺点：编译器不检测参数类型
connect(tea, SIGNAL(hungry(QString)), stu, SLOT(treat(QString)))
```


lambda表达式
```cpp
// 语法
[=](){/*函数体*/}
// 使用
connect(tea, SIGNAL(hungry(QString)), [=](){
    treat()
    });

```