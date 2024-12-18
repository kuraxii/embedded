# 内存的初始化过程
开启mmu，初始化内核页表，管理物理内存
## aarch64 虚拟内存布局
## 页表初始化 page_init

页表初始化分为两步

- map_kernel() 内核映像映射
- map_mem() 物理内存的线性映射

调用流程
```
start_kernel
    setup_arch
        paging_init
            map_kernel
            map_mem
```
# map kernel
该函数对内核映像的各个段分别进行映射，映射到内核空间的虚拟地址为vmalloc区域
分别映射如下段
代码段
只读数据段
初始化代码段
初始化数据段
数据段