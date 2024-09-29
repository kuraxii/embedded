# C/C++ 安全规则集合

使资源接受对象化管理

将资源托管于类的对象，使资源的生命周期协同于对象的生命周期，避免分散处理分配与回收等问题，是 C++ 程序设计中的重要方法。

动态申请的资源如果只能通过普通指针或变量访问，不受对象的构造和析构等机制控制，则称为“无主”资源，极易产生泄漏或死锁等问题。应尽量使用标准库提供的容器、智能指针以及资源对应的类，避免直接使用 new、delete 以及底层资源管理接口。
示例
```c
class FILE_FD {
  struct FD_DELETER {
    using pointer = int *;
    void operator()(pointer p) {
      if (*p >= 0) {
        close(*p);
        std::cout << "close...\n";
      }
    }
  };

  std::unique_ptr<int, FD_DELETER> uptr;

public:
  FILE_FD(const char *path, int WR) {
    int fd = open(path, WR);
    uptr = std::unique_ptr<int, FD_DELETER>(new int{fd});
    std::cout << "open...\n";
  }
  int *get() const { return uptr.get(); }
};
```