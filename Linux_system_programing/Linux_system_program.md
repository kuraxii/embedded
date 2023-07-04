[TOC]

# Linux 系统编程

## time
```c
#include <time.h>
time_t time(time_t *tloc);
// 功能：计算从1970-1-1 00:00:00	到现在过了多少秒
// 返回值：成功返回这个秒数
// tloc：作为实参也可以获取秒数
```

时间格式
```c
char *ctime(const time_t *timep);
// 功能：把过了多少秒的数据直接转换成年月日这种格式的字符串
// 返回值：成功返回字符串的地址，失败返回NULL
// timep：通过time函数得到的秒数

char *asctime(const struct tm *tm);
// 功能：将存储为结构的时间转换为字符串
// 返回值：成功返回字符串的地址，失败返回NULL
// tm：时间结构体
// 		struct tm {
//                int tm_sec;    /* Seconds (0-60) */
//                int tm_min;    /* Minutes (0-59) */
//                int tm_hour;   /* Hours (0-23) */
//                int tm_mday;   /* Day of the month (1-31) */
//                int tm_mon;    /* Month (0-11) */
//                int tm_year;   /* Year - 1900 */
//                int tm_wday;   /* Day of the week (0-6, Sunday = 0) */
//                int tm_yday;   /* Day in the year (0-365, 1 Jan = 0) */
//                int tm_isdst;  /* Daylight saving time */
//            };
```
```c
struct tm *gmtime(const time_t *timep);
// 功能：转换时间格式，把秒数转换成结构体（世界标准时间）
// 返回值：成功返回结构体的地址，失败NULL
// timep：time函数得到的秒数


struct tm *localtime(const time_t *timep);
// 功能：转换时间格式，把秒数转换成结构体（本地时间）
// 返回值：成功返回结构体的地址，失败NULL
// timep：time函数得到的秒数

size_t strftime(char *buf, size_t max, const char *format, const struct tm *tm);
// 功能：转换时间格式，将结构体中的时间数据格式化转换为字符串
// 返回值,成功返回字符串大小 失败-1设置errno

char *strptime(const char *s, const char *format, struct tm *tm);
// 功能：将字符串格式化转换为tm中的各种值
```


## 文件I/O

```c
open #include <fcntl.h>
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
//flag常用参数(使用头文件fcntl.h)
// O_RDONLY  O_WRONLY  O_RDWR #读 写 读写权限 
// O_APPEND 以追加的方式打开
// O_CREAT  文件不存在则创建
// O_EXCL  和creat一起用，文件不存在报错
// O_TRUNC  从0截断文件
// O_DIRECTORY 如果不是目录则出错
// O_NONBLOCK  设置非阻塞
// O_DIRECT 跳过内核缓冲区高速缓存
//创建文件时，指定文件访问权限。权限同时受umask影响。结论为

//文件权限 = mod & ~umask

//常见错误
//1.打开文件不存在
//2.以写方式打开只读文件
//3.以只写方式打开目录

错误处理函数 与errno相关

```

### read write

```c
ssize_t read(int fd,void *buf,size_t count)
// 参数（头文件 unistd.h）
// fd 文件描述符
// buf 存数据的缓冲区
// count 缓冲区的大小
// 返回值
// 成功 读到的字节数
// 失败 -1，设置errno
// 如果 -1，并且 errno = EAGIN 或 EWOULDBLOCK吗，说明不是read失败，而是read在以非阻塞方式读设备文件或网络文件 

//read返回值补充
// 成功：
//   >0 实际读到的字节数
//   =0 阻塞文件读到末尾（对端已关闭）


```

```c
ssize_t write(int fd,const void *buf,size_t count)
// 参数（头文件 unistd.h）
// fd 文件描述符
// buf 待写入数据的缓冲区
// count 写入数据的大小
// 返回值
// 成功 写入的字节数
// 失败 -1，设置errno
```

```c
/*
 *./mycp  命令行参数实现简单的cp命令
 */

char buf[1024];

int main(int argc, char *argv[])
{
 int src, dst;
 int n;

 src = open(argv[1], O_RDONLY); //只读打开源文件
 if(src < 0){
  perror("open src error");
  exit(1);
 }
 //只写方式打开，覆盖原文件内容，不存在则创建，rw-r--r--
 dst = open(argv[2], O_WRONLY|O_TRUNC|O_CREAT, 0644);
 if(src < 0){
  perror("open dst error");
  exit(1);
 }
 while((n = read(src, buf, 1024))){
  if(n < 0){
   perror("read src error");
   exit(1);
  }
  write(dst, buf, n);  //不应写出1024, 读多少写多少
 }

 close(src);
 close(dst);

 return 0;
}

```

### 文件描述符

```shell
PCB(process control block) 进程控制块  本质：结构体
成员：文件描述符
文件描述符：0-1023    文件描述符使用表中可用的最小的
0 - STDIN_FILENO
1 - STDOUT_FILENO
3 - STDERR_FILENO
```

### 阻塞，非阻塞  

```shell
阻塞，非阻塞  是设备文件，网络文件的属性
 产生阻塞的场景。 读设备文件，读网络文件。(读常规文件无阻塞概念)
 /dev/tty

```

### fcntl 改打开文件属性

```c
int flags = int fcntl(int fd, int cmd, ... /* arg */ );
// cmd
//获取文件状态 F_GETFL
//设置文件状态 F_SETFL
```

demo

```c
#define MSG_TRY "try again\n"

int main(void)
{
    char buf[10];
    int flags, n;

    flags = fcntl(STDIN_FILENO, F_GETFL); //获取stdin属性信息
    if(flags == -1){
        perror("fcntl error");
        exit(1);
    }
    flags |= O_NONBLOCK;
    int ret = fcntl(STDIN_FILENO, F_SETFL, flags);
    if(ret == -1){
        perror("fcntl error");
        exit(1);
    }

tryagain:
    n = read(STDIN_FILENO, buf, 10);
    if(n < 0){
        if(errno != EAGAIN){  
            perror("read /dev/tty");
            exit(1);
        }
        sleep(3);
        write(STDOUT_FILENO, MSG_TRY, strlen(MSG_TRY));
        goto tryagain;
    }
    write(STDOUT_FILENO, buf, n);

    return 0;
}

```

### lseek函数

```c
off_t lseek(int fd, off_t offset, int whence);
// 参数
//   fd：文件描述符
//   offset：偏移量
//   whence：起始偏移位置： SEEK_SET/SEEK_CUR/SEEK_END(文件 起始位置/当前位置/末尾位置)
// 返回值
//   成功：较起始位置偏移量
//   失败：-1 errno
```

应用场景：

- 文件'读''写'使用同一偏移位置
- 使用lseek获取文件大小
- 使用lseek拓展文件大小
- 可以使用truncate 函数直接拓展文件 int ret = truncate("dict.cp",250); 将文件dict.cp拓展到250字节

查看文件
od -tcx <filename> 查看文件的16进制表示形式
od -tco <filename> 查看文件的10进制表示形式

使用

```c
int main(void)
{
 int fd, n;
 char msg[] = "It's a test for lseek\n";
 char ch;

 fd = open("lseek.txt", O_RDWR|O_CREAT, 0644);
 if(fd < 0){
  perror("open lseek.txt error");
  exit(1);
 }

 write(fd, msg, strlen(msg));    //使用fd对打开的文件进行写操作，问价读写位置位于文件结尾处。

 lseek(fd, 0, SEEK_SET);  //偏移到文件起始位置       //修改文件读写指针位置，位于文件开头。 注释该行会怎样呢？ 会导致无法读到数据

 while((n = read(fd, &ch, 1))){
  if(n < 0){
   perror("read error");
   exit(1);
  }
  write(STDOUT_FILENO, &ch, n);   //将文件内容按字节读出，写出到屏幕
 }

 close(fd);

 return 0;
}
```

获取文件大小

```c
int main(int argc,char *argv[])
{
    int fd = open(argv[1],O_RDONLY);
    if(fd == -1){
        perror("open file fail!");
        exit(1);
    }
    int len = lseek(fd,0,SEEK_END);    //偏移位置到文件末尾
    printf("lseek get len = %d\n",len);



    return 0;
}
```

拓展文件大小

```c
int main(int argc,char *argv[])
{
    int fd = open(argv[1],O_RDWR);
    if(fd == -1){
        perror("open file fail!");
        exit(1);
    }
    int len = lseek(fd,110,SEEK_END);   //向文件末尾偏移110
    printf("lseek get len = %d\n",len);
    printf("%ld\n",write(fd, "a", 1));

    close(fd);

    return 0;
}
```

### ioctl函数

对设备的I/O通道进行管理，控制设备特性

### 传入传出参数

- 传入参数

1. 指针作为函数参数
2. 通常由const关键字修饰
3. 指针在有效区，在函数内部做读参数

- 传出参数

1. 指针作为函数参数
2. 在函数调用之前指针指向的空间可以无意义，但必须有效
3. 在函数内部做写操作
4. 函数调用结束后充当函数返回值

- 传入传出函数

1. 指针做函数参数
2. 在函数调用前，指针指向的空间有实际意义
3. 在函数内部，先做读操作，再做写操作
4. 函数调用中，充当函数返回值

例：

```c
char *strcpy(char *dest, const char *src);
//char *desk为传出参数   const char *src为传入参数

```

### stat与lstat函数

获取文件属性(从inode结构体中获取)

```c
int stat(const char *path, struct stat *buf);
int lstat(const char *path, struct stat *buf);
// 参数：
// path: 文件路径
// buf: (传出参数) 存放文件属性
// statbuf：获取的属性存放的结构体
// 		struct stat {
//                dev_t     st_dev;         /* ID of device containing file */
//                ino_t     st_ino;         /* Inode number */
//                mode_t    st_mode;        /* File type and mode */
//                nlink_t   st_nlink;       /* Number of hard links */
//                uid_t     st_uid;         /* User ID of owner */
//                gid_t     st_gid;         /* Group ID of owner */
//                dev_t     st_rdev;        /* Device ID (if special file) */
//                off_t     st_size;        /* Total size, in bytes */
//                blksize_t st_blksize;     /* Block size for filesystem I/O */
//                blkcnt_t  st_blocks;      /* Number of 512B blocks allocated */

//                struct timespec st_atim;  /* Time of last access */
//                struct timespec st_mtim;  /* Time of last modification */
//                struct timespec st_ctim;  /* Time of last status change */

//            #define st_atime st_atim.tv_sec      /* Backward compatibility */
//            #define st_mtime st_mtim.tv_sec
//            #define st_ctime st_ctim.tv_sec
//           };
// 返回值:
//   成功 0
//   失败 -1 errno
// 区别：穿透符号链接：stat：会  lstat：不会
```



获取文件大小：buf.st_size

```c
//获取文件大小
int main(int argc,char *argv[])
{ 
  struct stat sbuf;
  
  int ret = stat(argv[1],&sbuf);
  if(ret == -1){
    perror("stat error");
    exit(0);
  }
  printf("file size : %ld \n",sbuf.st_size);
    return 0;
}
```

判断文件类型，权限
上述的文件类型中定义了检查这些类型的宏定义：

* S_ISLNK (st_mode)  判断是否为符号连接
* S_ISREG (st_mode)  是否为一般文件
* S_ISDIR (st_mode)  是否为目录
* S_ISCHR (st_mode)  是否为字符装置文件
* S_ISBLK (s3e)    是否为先进先出
* S_ISSOCK (st_mode)  是否为socket

```c
//判断是否是目录
int main(int argc,char *argv[])
{ 
  struct stat sbuf;
  
  int ret = stat(argv[1],&sbuf);
  if(ret == -1){
    perror("stat error");
    exit(0);
  }
  if(S_ISDIR(sbuf->st_mode)){ //判断
      printf("it is dir");
  }
    
    
  return 0;
}
```

### link和ulink函数

```c
int link(const char* oldpath, const char* newpath); //创建硬链接
int ulink(const char *pathname);  //删除dentry
// 返回值
// 成功 0
// 失败 -1 设置errno

```

使用link ulink函数实现mv命令

```c
int main(int argc,char *argv[])
{
    link(argv[1],argv[2]);    //建立argv[1]的硬链接到argv[2]
    ulink(argv[1]);     //删除argv[1]的dentry
    return 0;
}
```

注意linux下删除文件的机制：不断将st_nlink -1,直至减为0为止。无目录项对应的文件，将会被操作系统择机释放
因此，我们删除文件，从某种意义上说，只是让文件具备了被释放的条件

unlink函数的特征：清楚文件时，如果文件硬链接数到0了，没有dentry对应，但文件不会马上被释放。要等到所有打开该文件的进程关闭了该文件，系统才会挑时间将该文件释放掉

```c
int main(void)
{
 int fd, ret;
 char *p = "test of unlink\n";
 char *p2 = "after write something.\n";

 fd = open("temp.txt", O_RDWR|O_CREAT|O_TRUNC, 0644);
 if(fd < 0){
  perror("open temp error");
  exit(1);
 }

 ret = unlink("temp.txt");   //具备了被释放的条件
 if(ret < 0){
  perror("unlink error");
  exit(1);
 }

 ret = write(fd, p, strlen(p));
    if (ret == -1) {
        perror("-----write error");
    }

 printf("hi! I'm printf\n");
 ret = write(fd, p2, strlen(p2));
    if (ret == -1) {
        perror("-----write error");
    }

    printf("Enter anykey continue\n");
    getchar();

    p[3] = 'H';


 close(fd);


 return 0;
}

```

隐式回收：当进程结束运行时，所有该进程打开的文件会被关闭，申请的内存空间会被释放。被称为隐式回收系统资源

### getcwd和chdir函数

### 文件和目录权限

打开文件夹需要x权限

#### 目录操作函数 opendir、closedir、readdir

```c
//#include <dirent.h>
//#include <sys/types.h>
DIR *opendir(const char* name); //打开文件夹  成功返回DIR*  失败返回NULL
int closedir(DIR *dirp);  //关闭文件夹 
struct dirent *readdir(DIR* dirp); //读文件夹  获取目录     失败返回NULL

// struct dirent {
//        ino_t          d_ino;       /* Inode number */
//        off_t          d_off;       /* Not an offset; see below */
//        unsigned short d_reclen;    /* Length of this record */
//        unsigned char  d_type;      /* Type of file; not supported
//                                       by all filesystem types */
//        char           d_name[256]; /* Null-terminated filename */
//    };
```

###### 递归遍历目录

```c
void isfile(char *name);
void read_dir(char *dir);
void read_dir(char *dir)
{
  char path[256];
  DIR *dp;
  struct dirent *sdp;
  // 打开目录
  dp = opendir(dir);
  if (dp == NULL)
  {
    perror("opendir err");
    return;
  }

  // 读取目录项
  while ((sdp = readdir(dp)) != NULL)
  {
    // 去除掉 . .. 目录，以防止无线递归
    if (!strcmp(sdp->d_name, ".") || !strcmp(sdp->d_name, ".."))
    {
      continue;
    }
    // 目录项本身不可访问，故需要拼接
    if (strlen(dir)+strlen(sdp->d_name)+2 > 256)
    {
      perror("目录名错误");
      return;
    }
    
    sprintf(path, "%s/%s", dir, sdp->d_name);

    // 判断目录项是否是目录
    isfile(path);
  }
  closedir(dp);
  return;
}
void isfile(char *name)
{
  int ret;
  struct stat sbuf;
  // 获取文件路径信息
  ret = stat(name, &sbuf);
  if (ret == -1)
  {
    perror("stat err");
    return;
  }
  // 判断是否是文件
  if (S_ISDIR(sbuf.st_mode))
  {
    read_dir(name);
  }
  // 打印出文件名
  printf("%-50s\t\t%ld\n", name, sbuf.st_size);
  return;
}

int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    isfile(".");
  }
  else
  {
    isfile(argv[1]);
  }
  return 0;
}
```

### dup(重定向)

```c
int dup(int oldfd);  //返回值为新的文件描述符
int dup2(int oldfd,int newfd); //dupto 使oldfd覆盖new指向的文件，并关闭newfd newfd->oldfd oldfd必须为有效的文件描述符 返回值为newfd
// 使用dup得到的的新的文件描述符均指向同一个文件表
```

example

```c
DUP
int main(int argc,char *argv[])
{
  int fd;
  fd = open(argv[1],O_RDONLY);
  printf("fd = %d\n",fd);
  int newfd;
  newfd = dup(fd);
  
  printf("newfd = %d\n",newfd);
  close(fd);


  return 0;
}
```

```c
DUP2
int main(int argc,char *argv[])
{
  int fd1,fd2;
  fd1 = open(argv[1],O_RDWR);
  fd2 = open(argv[2],O_RDWR);
  printf("fd1 = %d fd2 = %d\n",fd1,fd2);
  int ret;
  ret = dup2(fd1,fd2);
  printf("fd1 = %d fd2 = %d\n",fd1,fd2);
  printf("%d\n",ret);
  dup2(fd1,STDOUT_FILENO) ;
  close(fd1);
  return 0;
}
```

###### fcntl函数实现dup函数

cmd：F_DUPFD
参数3：被占用的，返回最小可用的
      未被占用的，返回=该值的文件描述符

```c
int main(int argc,char *argv[])
{ 
  int fd1 = open(argv[1],O_RDWR);
  printf("fd1 = %d\n",fd1);
  int newfd = fcntl(fd1,F_DUPFD,0);//0被占用，fcntl使用文件描述符表中可用的最小的文件描述符返回
  printf("newfd = %d\n",newfd);
  int newfd2 = fcntl(fd1,F_DUPFD,7);//使用未被占用>=7的文件描述符
  printf("newfd2 = %d\n",newfd2);
  return 0;
}
```

## 进程控制

程序：死的，只占用磁盘空间
进程：活得，运行起来的程序。占用内存，cpu等系统资源

## 进程相关命令

```shell
ps # 显示进程

top 

nice # Linux nice命令以更改过的优先序来执行程序，如果未指定程序，则会印出目前的排程优先序，内定的 adjustment 为 10，范围为 -20（最高优先序）到 19（最低优先序）

renice # 新调整进程执行的优先

kill   # 发送信号

bg  # 用于将作业放到后台运行，使前台可以执行其他任务

fg # 用于将后台运行的或挂起的任务（或作业）切换到前台运行


jobs # 显示后台程序

```

### 虚拟内存和物理内存的映射关系

```https://www.bilibili.com/video/BV1KE411q7ee?p=77&spm_id_from=pageDriver&vd_source=a1edf3ad63e77272acbd0c1f89c365d6```

### pcb进程控制块

每个进程在内核中都有一个进程控制块（PCB）来维护进程相关信息，Linux内核的进程控制块是task_struct结构体
/usr/src/linux-headers-3.16.0-30/include/linux/sched.h文件中可以查看struct task_struct结构体定义。其内部成员有很多，我们重点掌握以下部分即可:

- 进程id (PID) 命令：ps aux可查看
- 进程的状态 （（初始态）， 就绪，运行，挂起，终止等状态）
- 进程切换时需要保存和恢复的一些cpu寄存器
- 描述虚拟地址空间的信息
- 当前工作目录
- *umask掩码
- 文件描述符表
- 和信号相关的信息
- 用户id和组id
- 会话（session）和进程组
- 进程可使用的资源上限（Resource Limit）

### 环境变量

- PATH

- SHELL 查看当前shell
- TERM  查看当前终端
- LANG  查看当前语言
- HOME  查看当前用户家目录

### fork函数

```c
//include <unistd.h>
pid_t fork(void);  //创建一个子进程 
//两个返回值
//成功
//父进程 返回子进程PID
//子进程 0
 -1 创建失败
 0  创建成功
 1  
```

example

```c
int main(int argc,char *argv[])
{

  printf("before fork-1-\n");
  printf("before fork-2-\n");
  printf("before fork-3-\n");
  printf("before fork-4-\n");
  pid_t pid = fork();
  if(pid ==-1){
    perror("fork error");
    exit(1);
  }else if(pid == 0){
    printf("---child is create\n");
  }else if (pid > 0)
  {
    printf("---parent press: my child is %d\n",pid);
  }
  printf("end of file\n");

  return 0;
}
```

#### getpid & getppid函数

```c
pid_t getpid(void);  //返回当前进程的pid
pid_t getppid(void); //返回父进程的pid
```

#### 循环创建进程

```c
int main(int argc,char *argv[])
{
  printf("---before create\n");
  printf("---before create\n");
  printf("---before create\n");
  size_t count;
  size_t i;
  pid_t pid;
  scanf("%ld",&count);
  for (i = 0; i < count; i++)
  {
    pid = fork();
    if(pid == -1)
      sys_err("fork err"); 
    if(pid == 0)
      break;
  }
  if(i == count){
    sleep(count);
    printf("parent process\n");
  }
  else{
    sleep(i);
    printf("%ld i am son process\n",i+1);
  }
  return 0;
}
```

#### 进程共享

| 父子相同之处 | 父子不相同之处 | 父子进程共享 |
| :----------: | :------------: | :------------: |
|   全局变量   | 进程id         | 文件描述符   |
|    .data     | fork返回值     | mmap建立的映射区 |
|    .text     | 父进程id       | （读时共享，写时复制）---大多应用于全局变量 |
|      栈      | 进程创建运行时间   |    |
|      堆      | 闹钟（定时器） |  |
|   环境变量   | 未决信号集     |      |
|    用户ID    |                |                |
|   宿主目录   |                |                |
| 进程工作目录 |                |                |
| 信号处理方式 |                |                |
|              |                |                |

父子进程间遵循**读时共享写时复制**原则。这样设计，无论子进程执行父进程的逻辑还是自己的逻辑都能节省内存开销  

fork之后父子进程运行顺序由操作系统的调度算法决定

### exec函数族

fork创建的程序执行的是父进程相同的程序（但又可能执行不同的代码分支），子进程往往要调用一种exec函数以执行另一个程序。当进程调用一种exec函数时，该进程的用户空间代码和数据完全被新程序替换，从新程序的启动开始执行。调用exec并不会创建新进程，所以调用exec前后台该进程的id并未改变。

将当前进程的.text、.data替换为所要加载的程序的.text、.data,然后让进程从新的.text第一天指令开始执行，但进程id不变，换核不换壳

```c  
int execl(const char *pathname, const char *arg, .../* 可写多个参数 */);
int execlp(const char *file, const char *arg, .../* 可写多个参数 */, NULL/* 结束符 */);
int execle(const char *pathname, const char *arg, .../* 可写多个参数 */);
int execv(const char *pathname, char *const argv[]);
int execvp(const char *file, char *const argv[]); 
int execvpe(const char *file, char *const argv[],char *const envp[]);

// 可执行文件查找方式
// 表中的前四个函数的查找方式都是指定完整的文件目录路径，而最后两个函数(以p结尾的函
// 数)可以只给出文件名，系统会自动从环境变量“$PATH”所包含的路径中进行查找。

// 参数表传递方式
// 两种方式：逐个列举或是将所有参数通过指针数组传递
// 以函数名的第五位字母来区分，字母为“l”(list)的表示逐个列举的方式；字母为
// “v”(vertor)的表示将所有参数构造成指针数组传递，其语法为char *const argv[]

// 环境变量的使用
// exec函数族可以默认使用系统的环境变量，也可以传入指定的环境变量。这里，以
// “e”(Enviromen)结尾的两个函数execle、execve就可以在envp[]中传递当前进程所使用的
// 环境变量
```

#### execlp函数

加载一个进程，借助PATH环境变量

#### exexl函数

加载一个进程，借助路径 （相对路径，绝对路径）

```c
int execl(const char *pathname, const char *arg, .../* 可写多个参数 */);
//成功：无返回
//失败：返回-1

```

```c
int execlp(const char *file, const char *arg, .../* (char  *) NULL */); 
//成功：无返回
//失败：返回-1
//参数1:要加载的程序的名字。该函数需要配合PATH环境变量来使用，当PATH中所有目录搜索后没有参数1则出错返回。
//该函数通常用来调用系统程序。如:ls、date、cp、 cat等命令。
```

### 孤儿进程

孤儿进程:父进程先于子进程结束，则子进程成为孤儿进程，子进程的父进程成为init进程，称为init进程领养孤儿进程。

### 僵尸进程

僵尸进程:进程终止，父进程尚未回收，子进程残留资源（PCB）存放于内核中，变成僵尸（zombie）进程。
特别注意，僵尸进程是不能使用kill命令清除掉的。因为 kill命令只是用来终止进程的,而僵尸进程已经终止。
思考!用什么办法可清除掉僵尸进程呢?  杀掉父进程让init进程回收  wait函数回收

### wait函数

一个进程在终止时会关闭所有文件描述符，释放在用户空间分配的内存，但它的 PCB还保留着，内核在其中保存了一些信息:如果是正常终止则保存着退出状态，如果是异常终止则保存着导致该进程终止的信号是哪个。这个进程的父进程可以调用wait或 waitpid_获取这些信息，然后彻底清除掉这个进程。我们知道一个进程的退出状态可以在 shell中用特殊变量$?查看，因为shell是它的父进程，当它终止时shell 调用wait或waitpid得到它的退出状态同时彻底清除掉这个进程。
父进程调用wait函数可以回收子进程终止信息。该函数有3个功能：

1. 阻塞等待子进程退出
2. 回收子进程残留资源
3. 回去子进程结束状态（退出原因）

```c

#include <sys/types.h>
#include <sys/wait.h>v
pid_t wait(int *status); 
/*函数作用 1.阻塞等待子进程退出
          2.清理子进程残留的pcb资源
          3.通过传出参数，得到走进从结束状态
          */
//返回 -1表示失败
//传出参数 status 表示子进程退出状态 
//运行时会阻塞，当子进程结束时才会继续运行

pid_t waitpid(pid_t pid, int *status, int options);
//参数 pid
 >0 回收指定pid的子进程
 -1 回收任意子进程 
 0 回收和当前调用 waitpid 同组的所有子进程
        

//options参数
//0 阻塞

// WNOHANG 如果pid指定的子进程没有结束，则waitpid()函数立即返回0，而不是阻塞在这个函数上等待；如果结束了，则返回该子进程的进程号。

// WUNTRACED 如果子进程进入暂停状态，则马上返回。
// 这些参数可以用“|”运算符连接起来使用。
// 如果waitpid()函数执行成功，则返回子进程的进程号；如果有错误发生，则返回-1，并且将失败的原因存放在errno变量中。
// 失败的原因主要有：没有子进程（errno设置为ECHILD），调用被某个信号中断（errno设置为EINTR）或选项参数无效（errno设置为EINVAL）

                    
//返回值
// >0 表示成功回收的子进程
// 0 表示函数调用时，参数3 指定了WNOHANG， 并且没有子进程结束
// -1 失败，设置errno

//宏函数
WIFEXITED(status);  //为非0 -> 进程正常结束
  WEXITSTATUS(status); //如上宏为非0 -> 获取进程退出状态

WIFSIGNALED(status); //为非0 -> 进程异常终止
  WTERMSIGN(status);  //如上宏为真-> 取得进程终止的那个信号

WIFSTOPPED(status);   //为非0 -> 进程暂停
  WSTOPSIG(status);  //如上宏为真 -> 取得进程暂停的那个信号
  WIFCONTINUED(status);   //为非0 -> 进程暂停后已经继续运行



```

explame

```c
//wait
int main(int argc,char *argv[])
{
  pid_t pid,wpid;
  int status;
  pid = fork();
  if(pid == 0)
  {
    printf("---child,my id = %d ,going to sleep 10s\n",getpid);
    sleep(10);
  }else if(pid > 0)
  {
    wpid = wait(&status);
    if(wpid == -1)
    {
      perror("wait err");
      exit(1);
    }
    printf("wait success");
  }else
  {
    perror("fork err");
    exit(1);
  }
  return 0;
}
```

```c
//waitpid回收子进程
int main(int argc,char *argv[])
{
  pid_t pid,wpid;
  int status;
  pid = fork();
  if(pid == 0)
  {
    printf("---child,my id = %d ,going to sleep 10s\n",getpid());
    sleep(100);
  }else if(pid > 0)
  {
    wpid = waitpid(pid,&status,0);
    if(waitpid == -1){
      perror("waitpid err");
      exit(0);
    }
    printf("i am parent i am kill %ld\n",wpid);
    if(WIFEXITED(status)){
      printf("the son prosses exit nor %d\n",WEXITSTATUS(status));
    }
    if(WIFSIGNALED(status)){
      printf("the son prosses exit nonor %d\n",WTERMSIG(status));
    }
    if(WIFCONTINUED(status)){
      printf("the son prosses stop %d\n",WSTOPSIG(status));     
    }
  }else
  {
    perror("fork err");
    exit(1);
  }
  return 0;
}
```

```c
//回收多个子进程
int main(int argc,char *argv[])
{
  int i;
  pid_t pid,wpid;

  for(i = 0;i < 5; i++){
    pid = fork();
    if(pid == 0){
      break;
    }
  }
  if(i == 5){
    // while((wpid = waitpid(-1,NULL,0))){  //阻塞方式回收子进程 
     
    //   printf("wait child %d \n",wpid);
    // }

    while(((wpid = waitpid(-1,NULL,WNOHANG))) != -1){  //非阻塞方式回收子进程 
      
      if(wpid > 0){
        printf("wait child %d \n",wpid);
      }else if (wpid == 0)
      {
        printf("no child die\n");
      }
      
      sleep(1);
    }
   
  }else{
    sleep(i);
    printf("i am child， pid = %d\n",getpid());
  }
  return 0;
}


```

### exit函数



## 进程通信

现用常用的进程间通信方式

1. 管道（使用最简单）
2. 信号（开销最小）
3. 共享映射区（无血缘关系）
4. 本地套接字（最稳定）

### 管道

管道是一种最基本的IPC(Inter Process Communication)机制，作用于有血缘关系的进程之间，完成数据传递。调用pipe系统函数即可创建一个管道。有如下特质:

1. 本质是一个伪文件(实为内存缓冲区)
2. 由两个文件描述符引用，一个表示读端，一个表示写端
3. 规定数据由写端流入管道，由读端流出

管道的原理：管道实为内核使用环形队列机制，借助内核缓冲区(4k)实现

管道的局限性：

1. 数据不能自己写，自己读
2. 管道中的数据不可反复读取。一旦读走，管道不服存在
3. 采用半双工通信方式，数据只能在单方向上流动
4. 只能在有公共祖先的进程间使用管道
只能在有公共祖先的进程间使用管道。

```c
#include <unistd.h>
int pipe(int pipefd[2]);   //创建并打开管道
//参数：fd[0] 读端
//     fd[1] 写端
//返回值：成功 0
//       失败 -1 errno
```

管道的读写行为

- 读管道
  - 管道有数据，read返回实际读到的数据
  - 管道无数据
    - 无写端 raed返回0 （类似读到文件尾）
    - 有写端，read阻塞等待
- 写管道
  - 管道读端全部被关闭，进程异常终止（也可以使用捕捉 SIGPIPE 信号，使进程不终止）
    - 管道已满:write阻塞
    - 管道未满，write将数据写入，并返回实际写入的字节数

```c
void sys_err(char *str){
  perror(str);
  exit(1);
}
int main(int argc,char *argv[])
{

  int ret;
  pid_t pid;
  int pipefd[2];
  ret = pipe(pipefd);
  char *str = "hello pipe\n";
  char buf[1024];

  pid = fork();
  if(pid > 0){   //父进程
    close(pipefd[0]);
    write(pipefd[1],str,strlen(str));
    close(pipefd[1]);
  }else if (pid == 0){ //子进程
    close(pipefd[1]);
    ret = read(pipefd[0],buf,sizeof(buf));   //写要控制写入大小，否则多余内容输出乱码
    write(STDOUT_FILENO,buf,ret);
    close(pipefd[0]);
   
  }else{
    sys_err("fork err");
  }
  return 0;
}
```

### fifo命名管道

fifo命名管道的创建
命令行方式

```shell
mkfifo <filenale>
```

函数方式

```c
int mkfifo(const char *pathname, mode_t mode);
// 参数：
// pathname：文件名
// mode： 文件权限

// 返回值：
// 成功：返回文件描述符
// 失败：-1
```

example

```c
//读端
void sys_err(char *str){
  perror(str);
  exit(1);
}

int main(int argc,char *argv[])
{
  char buf[1024];
  int ret;
  int fd = open(argv[1],O_RDONLY);
  if(fd < 0){
    sys_err("open error");
  }
  while(1){
    ret = read(fd,buf,sizeof(buf));
    
    write(STDOUT_FILENO,buf,ret);
   sleep(1);
  }
  return 0;
}
```

```c
//写端
int main(int argc,char *argv[])
{
  char buf[1024];
  int ret,i = 0;
  int fd = open(argv[1],O_WRONLY);
  if(fd < 0){
    sys_err("open error");
  }
  char *str = "fifo_w";
  while(1){
    i++;
    printf("%d\n",i);
    sprintf(buf,"%d %s\n",i,str);
    write(fd,buf,strlen(buf));
    sleep(2);
  }
  close(fd);
  return 0;
}
```

### 存储映射

#### mmap函数原理

```c
#include <sys/mman.h>
void *mmap(void *addr, size_t length, int prot, int flags,int fd, off_t offset);  //创建共享内存映射区
//参数 
// addr 指定映射区的首地址。通常传NULL，表示系让统自动分配
// length 共享内存映射区大小  
// prot 共享内存映射区的读写属性 PROT_READ、PROT_WRITE、PROT_READ|PROT_WRITE
// flags 标注共享内存的共享属性 MAP_SHARED MAP_ORIVATE
// fd 用于创建内存映射区的那个文件的 文件描述符
// offset 偏移位置  4k(4096byte)的整数倍   
//返回值 
// 成功 映射区的首地址
// 失败 -1 errno  

int munmap(void *addr, size_t length); //释放共享内存映射区
//参数
// addr （映射区的首地址）mmap的返回值
// length 映射区的大小

```

#### mmap注意事项

1. 用于创建映射区的大小为0，实际指定非0大小创建有映射区，出"总线错误" bus error
2. 实际制定0大小创建映射区，出"无效参数错误" Invalid argument  (映射区大小不能为0)
3. 用于创建映射区的文件读写行为为 只读 ，映射区属性为读写。出"无效参数错误" Invalid argument
4. 创建映射区，打开的文件需要read权限。
5. 文件描述符fd, 再mmap创建银行社区完成即可关闭。后访问文件，用地址访问
6. offset(偏移量) 必须是 4096的整数倍
7. 对申请的映射区内存，不能越界访问
8. mmap用于释放的地址必须是mmap申请返回的地址（mmap首地址）
9. 映射区访问权限为“私有”MAP_PRIVATE，对内存所做的所有修改，只在内存有效，不会反应到物理磁盘上。

mmap函数的保险调用方式
  1.open(O_RDER)
  2.mmap(NULL,<filesize>, PROT_READ |PROT_WRITE，MAP_SHARED,fd，0);

#### 父子进程使用mmap实现进程间通信

父进程先创建映射区。 open(O_RDWR) mmap()
指定MAP_SHARE 权限
fork创建子进程
一个进程读 一个进程写

通过传递指针实现

```c
int main(int argc,char *argv[])
{
  pid_t pid;
  char *p;
  int ret,fd;
  if(argc < 2){
    sys_err("too few file input");
  }

  fd = open(argv[1], O_RDWR | O_CREAT , 0664);
  if(fd == -1){
    sys_err("open error");
  }
  int len = lseek(fd, 0, SEEK_END);
  if(len == 0){
    lseek(fd, 100, SEEK_END);
    write(fd, "\0", 1);
  }
  len = lseek(fd, 0, SEEK_END);
  p = (char*)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(p == MAP_FAILED){
       sys_err("mmap error");
  }
  pid = fork();
  if(pid == -1){
    sys_err("fork error");
  }
  if(pid == 0){ //子进程
    strcpy(p, "abcaaa");
  }
  if(pid > 0){  //父进程
    wait(NULL);
    // sleep(1);
    printf("i am parient  i read mmap: %s\n",p);
    munmap(p, len);
  }
  return 0;
}
```

#### 无血缘关系进程使用mmap实现进程间通信

通过打开相同文件实现
读

```c
int main(int argc,char *argv[])
{
  pid_t pid;
  char *p;
  int ret,fd;
  if(argc < 2){
    sys_err("too few file input");
  }
  
  fd = open(argv[1], O_RDWR | O_CREAT , 0664);
  if(fd == -1){ 
    sys_err("open error");
  }
  int len = lseek(fd, 0, SEEK_END);
  if(len == 0){
    lseek(fd, 100, SEEK_END);
    write(fd, "\0", 1);
  }
  len = lseek(fd, 0, SEEK_END);
  p = (char*)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(p == MAP_FAILED){
       sys_err("mmap error");
  }
    printf("i am read process  i read mmap: %s\n",p);
  return 0;
}
```

写

```c
int main(int argc,char *argv[])
{
  pid_t pid;
  char *p;
  int ret,fd;
  if(argc < 2){
    sys_err("too few file input");
  }
  fd = open(argv[1], O_RDWR | O_CREAT , 0664);
  if(fd == -1){
    sys_err("open error");
  }
  int len = lseek(fd, 0, SEEK_END);
  if(len == 0){
    lseek(fd, 100, SEEK_END);
    write(fd, "\0", 1);
  }
  len = lseek(fd, 0, SEEK_END);
  p = (char*)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(p == MAP_FAILED){
       sys_err("mmap error");
  }
  strcpy(p, "abcaaa");
  sleep(10);
  return 0;
}
```

### 信号

信号的共性：简单，不能携带大量信息，满足条件才能发送
信号是软件层面的`中断`。一旦信号产生，无论程序执行到什么位置，必须立刻停止运行，处理信号 ，处理结束，在继续执行后续命令
所有的信号的产生及处理全部都是由 内核 完成的

#### 与信号相关的事件与状态

产生信号

1. 按键产生 如：ctrl+c ctrl+z ctrl+\
2. 系统调用产生 如：kill raise abort
3. 软件条件产生 如：定时器 sleep
4. 硬件异常产生 如：非法访问内存(段错误) 除0(浮点数除外) 内存对齐出错(总线出错)
5. 命令产生 如：kill 命令

信号递达

- 递送并且到达进程

信号未决

- 信号产生和递达之间的状态主要由**阻塞**(屏蔽)导致该状态

信号的处理方式

1. 执行默认动作
2. 忽略(丢弃)
3. 捕捉(调用用户处理函数)

Linux内核的进程控制块PCB是一个结构体，task_struct,除了包含进程id，状态，工作目录,用户id，组id，文件描述符表，还包含了信号相关的信息，主要指阻塞信号集和未决信号集。

阻塞信号集(信号屏蔽字)
本质：位图，用来记录信号的屏蔽状态。一旦被屏蔽的信号，在被屏蔽之前，一直处于未决态

未决信号集
本质：位图。用来记录信号的处理状态。该信号集中的信号，表示已经产生但未被处理的信号

信号4要素：

- 信号编号
- 信号名称
- 信号事件
- 信号默认处理动作

信号使用之前，应先确定4要素，而后再使用

常规信号
这里特别强调了9 SIGKILL和 19 SIGSTOP信号，不允许忽略和捕捉，只能执行默认动作。甚至不能将其设置为阻塞。

1. `SIGHUB`  当用户退出 shell时，由该shell启动的所有进程将收到这个信号，默认动作为终止进程
2. `SIGINT`  当用户按下了<Ctrl+C>组合键时，用户终端向正在运行中的由该终端启动的程序发出此信号。默认动作为终止进程
3. `SIGQUIT`  当用户按下<ctrl+\>组合键时产生该信号，用户终端向正在运行中的由该终端启动的程序发出些信号。默认动作为终止进程
4. SIGILL  CPU检测到某进程执行了非法指令。默认动作为终止进程并产生core文件。
5. SIGTRAP  该信号由断点指令或其他trap指令产生。默认动作为终止里程并产生core文件。 gdb调试中会发生该信号
6. SIGABRT  调用abort函数时产生该信号。默认动作为终止进程并产生core文件。
7. `SIGBUS`  非法访问内存地址，包括内存对齐出错，默认动作为终止进程并产生core文件。
8. `SIGFPE`  在发生致命的运算错误时发出。不仅包括浮点运算错误，还包括溢出及除数为О等所有的算法错误。默认动作为终止进程并产生core文件。
9. **`SIGKILL`**  无条件终止进程。本信号不能被忽略，处理和阻塞。默认动作为终止进程。它向系统管理员提供可以杀死任何进程的方法。
10. `SIGUSR1`  用户定义的信号。即程序员可以在程序中定义并使用该信号。默认动作为终止进程   没有默认事件
11. `SIGSEGV`  指示进程进行了无效内存访问。默认动作为终止进程并产生core文件
12. `SIGUSR2`  另外一个用户自定义信号，程序员可以在程序中定义并使用该信号。默认动作为终止进程  没有默认事件
13. `SIGPIPE`  Broken pipe，向一个没有读端的管道写数据。默认动作为终止进程。
14. `SIGALRM`  定时器超时，超时的时间由系统调用alarm设置。默认动作为终止进程。
15. `SIGTERM`  程序结束信号，与SIGKILL不同的是，该信号可以被阻塞和终止。通常用来表示程序正常退出。 执行shell命令Kill时，缺省产生这个信号。默认动作为终止进程。
16. SIGSTKFLT  Linux早期版本出现的信号，现仍保留向后兼容。默认动作为终止进程。
17. `SIGCHLD`  子进程状态发生变化时，父进程会收到这个信号。默认动作为忽略这个信号
18. SIGCONT  如果进程已停止，则使其继续运行。默认动作为继续/忽略。
19. **`SIGSTOP`**  停止进程的执行。信号不能被忽略，处理和阻塞。默认动作为暂停进程。
20. SIGTSTP  停止终端交互进程的运行。按下<ctrl + z>组合键时发出这个信号。默认动作为暂停进程。
21. SIGTTIN  后台进程读终端控制台。默认动作为暂停进程。
22. SIGTTOU  该信号类似于SIGTTIN，在后台进程要向终端输出数据时发生。默认动作为暂停进程。
23. SIGURG  套接字上有紧急数据时，向当前正在运行的进程发出些信号，报告有紧急数据到达。如网络带清收据到达，默认动作为忽略该信号。
24. SIGXCPU  进程执行时间超过了分配给该进程的CPU时间﹐系统产生该信号并发送给该进程。默认动作为终止进程。
25. SIGXFSZ  超过文件的最大长度设置。默认动作为终止进程。
26. SIGVTALRM  虚拟时钟超时时产生该信号。类似于SIGALRM，但是该信号只计算该进程占用CPU的使用时间。默认动作为终止进程。
27. SGIPROF  类似于SIGVTALRM，它不公包括该进程占用CPU时间还包括执行系统调用时间默认动作为终止进程。
28. SIGWINCH  窗口变化大小时发出。默认动作为忽略该信号。
29. SIGIO  此信号向进程指示发出了一个异步lo事件。默认动作为忽略。·
30. SIGPWR  关机。默认动作为终止进程。
31. SIGSYS:无效的系统调用。默认动作为终止进程并产生core文件。
32. SIGRTMIN ～(64)SIGATMAX  LINUX的实时信号，它们没有固定的含义(可以由用户自定义)。所有的实时信号的默认动作都为终止进程。

#### kill函数与kill命令

kill命令
kill -<sign> <pid>
向指定pid的进程发送信号

kill函数

```c
#include <signal.h>
int kill(pid_t pid, int sig); 
// 参数：
// sig  信号 不推荐直接使用数字，应使用宏名，因为不同操作系统信号编号可能不同，但名称一致。   
// pid > 0  发送信号给指定的进程。
// pid = 0  发送信号给与调用kill函数进程属于同一进程组的所有进程。
// pid < -1  取|pid|发给对应进程组。
// pid = -1  发送给进程有权限发送的系统中所有进程。·
```

example

```c
int main(int argc,char *argv[])
{
  pid_t pid;
  pid = fork();
  if(pid == -1){ 
    sys_err("fork err");
  }
  if(pid == 0){
    printf("child pid = %d,ppid = %d\n",getpid(),getppid());
    kill(getppid(),SIGKILL);
  }else{
    printf("parent pid = %d\n",getpid());
  }
  return 0;
}
```

#### alarm函数   setitimer函数

time 命令 查看程序运行时间  实际时间 = 用户时间 + 内核时间 + 等待时间  --》优化瓶颈 I/O

alarm

```c
#include <signal.h>
unsigned int alarm(unsigned int seconeds);   //定时发送ALARM信号给进程 自然计时
//返回0或剩余秒数，无失败
// 常用：取消定时器alarm(0) ,返回旧闹钟剩余的秒数，无失败
// 定时与进程状态无关。就绪，运行，挂起，终止，僵尸。。。无论进程处于何种状态，alarm都计时
```

setitimer

```c

int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);
// 参数：which 指定计时方式
//      1. 自然定时  ITIMER_REAL -> SIGALRM   计算自然时间
//      2. 虚拟空间计时(用户空间) -> SIGVTALRM   只计算进程占用cpu的时间
//      3. 运行时计时(用户+内核) -> SIGPROF  计算占用cpu及执行系统调用的时间,
// new_val 定时秒数
// old_val 传出参数。上次定时剩余时间
    // it_interval 用来设定两次定时任务之间间隔的时间
    // it_value 定时的时长

// 成功 返回0
// 失败 -1 errno


```

深入理解setitimer
`itimeval`又是由两个`timeval`结构体组成，`timeval`包含`tv_sec`和`tv_usec`两部分，其中`tv_se`为秒，`tv_usec`为微秒(即1/1000000秒)
其中的new_value参数用来对计时器进行设置，`it_interval`为计时间隔，`it_value`为延时时长，下面例子中表示的是在`setitimer`方法调用成功后，延时1微秒便触发一次SIGALRM信号，以后每隔200毫秒触发一次`SIGALRM`信号。
`settimer`工作机制是，先对`it_value`倒计时，当`it_value`为零时触发信号，然后重置为`it_interval`，继续对`it_value`倒计时，一直这样循环下去。
基于此机制，`setitimer`既可以用来延时执行，也可定时执行。
假如`it_value`为0是不会触发信号的，所以要能触发信号，`it_value`得大于0；如果`it_interval`为零，只会延时，不会定时(也就是说只会触发一次信号)。
old_value参数，通常用不上，设置为NULL，它是用来存储上一次setitimer调用时设置的new_value值。

### 信号集操作函数

#### 信号集设定

```c
signset_t set;   //typedef unsigned long sigset t;
int sigemptyset(sigset .t *set);  //将某个信号集清0 成功:0;失败:-1
int sigfillset(sigset t*set);  //将整个信号集置1 成功:0;失败:-1
int sigaddset(sigset t *set, int signum);  //将某个信号加入信号集 成功:0﹔失败:-1
int sigdelset(sigset t *set, int signum);  //将某个信号移除信号集 成功:0﹔失败:-1
int sigismember(const sigset .t *set, int signum);  //判断某个信号是否在信号集中  返回值:在集合:1﹔不在:0  出错:-1 

sigset_t //类型的本质是位图。但不应该直接使用位操作，而应该使用上述函数，保证跨系统操作有效。
```

#### sigprocmask函数

用来屏蔽信号、解除屏蔽也使用该函数。其本质，读取或修改进程的 信号屏蔽字(PCB中)
**严格注意，屏蔽信号。只是将信号处理延后执行(延至解除屏蔽);而忽略表示将信号丢处理。**

```c
#include <signal.h>
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
// 参数： 
//   set: 传入参数，是一个信号集位图
//   oldset: 传出参数，保留旧的信号屏蔽集
//   how: 参数取值，假设当前的信号屏蔽字为mask
//     1. SIG_BLOCK  设置阻塞  当how设置为此值，set表示需要屏蔽的信号。相当于mask=mask | set
//     2. SIG_UNBLOCK  取消阻塞 当 how设置为此，set表示需要解除屏蔽的信号。相当于mask= mask & ~set
//     3. SIG_SETMASK  当how设置为此，set表示用于替代原始屏蔽及的新屏蔽集。相当于mask = set 若调用sigprocmask解除了对当前若干个信号的阻塞，则在sigprocmask返回前，至少将其中一个信号递达
// 返回值：
//     成功 0 
//     失败 -1 errno
```

#### sigpending函数

读取当前进程的未决信号集

```c
#include <signal.h>
int sigpending(sigset_t *set);
// 参数： 
//     set 传出参数
// 返回值：
//     成功 0
//     失败 -1 errno
```

#### 信号集处理案例

```c
void printset(sigset_t *set){

  for (size_t i = 0; i < 32; i++)
  {
    if(sigismember(set, i))
      putchar('1');
    else{
      putchar('0');
    }
  }
  printf("\n");

}

int main(int argc,char *argv[])
{
  int ret;
  sigset_t set, oldset, pedset;
  ret = sigemptyset(&set);   //设置为0
  if(ret == 1){
    sys_err("sigemptyset err");
  }
  ret = sigaddset(&set,SIGINT);  //添加set值
  if(ret == 1){
    sys_err("sigaddset err");
  }
  ret = sigprocmask(SIG_BLOCK, &set, NULL);   //添加set到屏蔽字中
  if(ret == 1){
    sys_err("sigprocmask err");
  }
  
  while(1){
    ret = sigpending(&pedset);  //读取当前未决信号集 
  if(ret == 1){
    sys_err("sigpending err");
  }
    printset(&pedset);   //打印set集
    sleep(1);
  }
  return 0;
}
```

### 信号捕捉

#### signal与sigaction函数

signal
`signal`函数用来在进程中指定当一个信号到达进程后该做什么处理，主要的两种方式有忽略某些信号，(监听到`SIGTERM`/`SIGINT`)退出前的打扫工作。信号处理函数的`handler`有两个默认值，分别是`SIG_IGN`和`SIG_DFL`，表示忽略和默认行为。而且`signal`函数是阻塞的，比如当进程正在执行`SIGUSR1`信号的处理函数，此时又来一个`SIGUSR1`信号，`signal`会等到当前信号处理函数处理完后才继续处理后来的`SIGUSR1`，不管后来的多少个`SIGUSR1`信号，统一看做一个来处理。还有`SIGKILL`和`SIGSTOP`这两个信号是`signal`函数捕捉不到的。

```c
#include <signal.h>
sighandler_t signal(int signum, sighandler_t handler);  //注册捕捉信号
// 参数：
//   signum 要捕捉的信号
//   hander 信号处理函数
//   sighandler_t 函数指针  原型 typedef void (*sighandler_t)(int);

```

sigaction

```c
#include <signal.h>
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
struct sigaction {
         void     (*sa_handler)(int); //信号处理函数
         void     (*sa_sigaction)(int, siginfo_t *, void *);
         sigset_t   sa_mask; //信号屏蔽集
         int        sa_flags; 
         void     (*sa_restorer)(void);// 已废弃
};
// 参数：
//   signum   要捕捉的信号
//   act  信号的处理参数
//   oldact  保存信号上次安装时的处理参数(备份的作用)

// 返回值：
//   成功 0
//   失败 -1
```

example

```c
// signal
void sys_sig(int signo){
  printf("----catch sign %d\n", signo);
}

int main(int argc,char *argv[])
{
  signal(SIGINT,sys_sig);
  signal(SIGQUIT, sys_sig);

while (1);
  return 0;
}
```

```c
// sigaction
void sys_sig(int signo){
  printf("----catch sign %d\n", signo);
  sleep(5);
}

int main(int argc,char *argv[])
{
  struct sigaction act,oldact;

  act.sa_handler = sys_sig;
  sigemptyset(&act.sa_mask);
  sigaddset(&act.sa_mask, SIGINT);
  act.sa_flags = 0;
  sigaction(SIGQUIT, &act, &oldact);
  while(1){}
  return 0;
}
```

使用信号通信实现子进程回收 （SIGCHLD 子进程改变状态(停止、继续、退出)时，发送该信号给父进程）

```c
void catch_child(int signo){
  int ret;
  while((ret = wait(NULL)) != -1){
    
    printf("---catch child pid = %d\n",ret);
  }
}
int main(int argc,char *argv[])
{
    int i,ret;
    pid_t pid;
    signal(SIGCHLD, catch_child);
    for (i = 0; i < 5; i++)
    {
      if((pid = fork()) == 0)
        break;

      if(pid == -1){
        sys_err("fork err");
      }
    }
    if(i == 5){   //父进程
      printf("i am parent my pid = %d\n",getpid());

      signal(SIGCHLD, catch_child);
      while(1);
    }else
    {
      printf("i am child my pid = %d\n",getpid());
    }
  return 0;
}
```

### 慢速系统调用中断

系统调用可分为两类:慢速系统调用和其他系统调用。

1．慢速系统调用:可能会使进程永远阻塞的一类。如果在阻塞期间收到一个信号，该系统调用就被中断,不再继续执行(早期);也可以设定系统调用是否重启。如，read、write、pause、wait...

2.其他系统调用:getpid、getppid、fork....
结合pause，回顾慢速系统调用:
慢速系统调用被中断的相关行为，实际上就是`pause`的行为:如 read

1. 想中断`pause`，信号不能被屏蔽。
2. 信号的处理方式必须是捕捉(默认、忽略都不可以)，中断后返回-1，设置`errno`为`EINTR`(表“被信号中断”)
3. 可修改sa_flags,参数来设置被信号中断后系统调用是否重启。`SA_INTERRURT` 不重启   `SA_RESTART` 重启。

### 进程守护

Daemon(精灵)进程，是Linux 中的后台服务进程，通常独立于控制终端并且周期性地执行某种任务或等待处理某些发生的事件。一般采用以d结尾的名字。
Linux后台的一些系统服务进程，没有控制终端，不能直接和用户交互。不受用户登录、注销的影响，一直在运行着，他们都是守护进程。如:预读入缓输出机制的实现;ftp服务器;nfs服务器等。
创建守护进程，最关键的一步是调用setsid函数创建一个新的Session，并成为Session leader

#### 进程组与会话

##### 进程组

进程组，也称之为作业。BSD于1980年前后向Unix中增加的一个新特性。代表一个或多个进程的集合。每个进程都属于一个进程组。在waitpid函数和kill函数的参数中都曾使用到。操作系统设计的进程组的概念，是为了简化对多个进程的管理。
当父进程，创建子进程的时候，默认子进程与父进程属于同一进程组。进程组ID = 第一个进程ID(组长进程)。所以，组长进程标识,其进程组ID = 其进程ID
可以使用kill -SIGKILL -进程组ID(负的) 来将整个进程组内的进程全部杀死。

组长进程可以创建一个进程组，创建该进程组中的进程，然后终止。只要进程组中有一个进程存在，进程组就存在，与组长进程是否终止无关。

进程组生存期:进程组创建到最后一个进程离开(终止或转移到另一个进程组)。一个进程可以为自己或子进程设置进程组ID

##### 创建会话

会话（多个进程组的集合）
创建一个会话需要注意以下6点注意事项;,

1. 调用进程不能是进程组组长，该进程变成新会话首进程(session header)
2. 该进程成为一个新进程组的组长进程。,
3. 需有root权限(ubuntu不需要)
4. 新会话丢弃原有的控制终端，该会话没有控制终端.
5. 该调用进程是组长进程，则出错返回，
6. 建立新会话时，先调用fork,父进程终止，子进程调用setsid

##### getsid函数

```c
pid_t getsid(pid_t pid);   //获取进程所属的会话ID
// 返回值
//   成功:返回调用进程的会话ID﹔
//   失败:-1，设置errno
//   pid为0表示察看当前进程session ID
```

ps ajx,命令查看系统中的进程。参数a表示不仅列当前用户的进程，也列出所有其他用户的进程，参数x表示不仅列有控制终端的进程，也列出所有无控制终端的进程，参数j表示列出与作业控制相关的信息。
组长进程不能成为新会话首进程，新会话首进程必定会成为组长进程。+

##### setsid函数

```c
pid_t setsid(void);   //创建一个会话，并以自己的ID设置进程组ID，同时也是新会话的ID
// 返回值
//   成功: 返回调用进程的会话ID
//   失败: -1，设置errno 
```

调用了setsid函数的进程，既是新的会长，也是新的组长。

#### 创建守护进程模型

1. 创建子进程，父进程退出
    fork()
    所有工作在子进程中进行形式上脱离了终端控制
2. 在子进程中创建新会话
    setsid()函数
    使子进程完全独立出来，脱离控制
3. 改变当前目录为根目录  根据需要
    chdir()函数 改变工作目录
    防止占用可卸载的文件系统，也可以换成其他路径
4. 重设文件权限掩码   根据需要
    umask()函数
    防止继承的文件创建屏蔽字拒绝某些权限
    增加守护进程的灵活性
5. 关闭文件描述符  根据需要
6. 开始执行守护进程核心工作守护进程，退出处理程序模型

## 线程

概念
进程  有独立的进程地址空间，有独立的pcb
线程  有独立的pcb 没有独立 的进程地址空间
查看线程  ps -Lf <pid>  ->线程号 lwp -->  cpu执行的最小单位

### linux内核线程实现原理

类Unix系统中，早期是没有“线程”概念的，80年代才引入，借助进程机制实现出了线程的概念，因此在这类系统中，进程和线程关系密切。

1. 轻量级进程(light-weight process)，也有PCB，创建线程使用的底层函数和进程一样，都是clone
2. 从内核里看进程和线程是一样的，都有各自不同的PCB，但是PCB中指向内存资源的三级页表是相同的。
3. 进程可以蜕变成线程
4. 线程可看做寄存器和栈的集合
5. 在linux 下，线程最是小的执行单位﹔进程是最小的分配资源单位。察看Lwp 号: ps  -Lf pid查看指定线程的lwp号。

### 线程共享资源

1. 文件描述符
2. 每种信号的处理方式
3. 当前工作目录
4. 用户id 和组 id
5. 内存地址空间(.text .data .bss .heap 共享库)

### 线程非共享资源

1. 线程id。
2. 处理器现场和栈指针(内核栈)
3. 独立的栈空间(用户空间栈)
4. errno变量
5. 信号屏蔽字
6. 调度优先级

## 线程控制原语

### pthread_self函数

```c
#include <pthread.h>
pthread_t pthread_self(void);   //获取线程ID。其作用对应进程中getpid()函数。
// 返回值:成功:调用该函数的线程ID
// 失败:无!
```

线程ID: pthread_t类型，本质:在Linux下为无符号整数(%lu)，其他系统中可能是结构体实现.线程ID是进程内部，识别标志。(两个进程间，线程ID允许相同)
注意:不应使用全局变量 pthread_t tid，在子线程中通过 pthread_create传出参数来获取线程ID，而应使用pthread_self。v

### pthread_create函数

创建一个新线程。  其作用，对应进程中 fork()函数。

```c
#include <pthread.h>
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
// 参数：
// thread  传出参数 指向线程id的指针
// attr  传入参数，通常传NULL，表示使 用线程默认属性。若想使用具体属性也可以修改该参数。
// start_routine  函数指针，指向线程主控函数（线程体），该函数运行结束，则线程结束。
// arg  线程主函数执行期间所使用的参数，若不同参数传NULL。
// 返回值：
//   成功 0
//   失败 错误号
//   ---Linux环境下，所有线程特点，失败均直接返回错误号
```

### pthread_exit函数

```c
#include <pthread.h>
void pthread_exit(void *retval);  //将单个线程退出  相当于return 
// 参数：
//   retval  表示线程退出状态 通常传NULL
```

线程中，禁止使用`exit`函数，会导致进程内所有线程全部退出。
在不添加`sleep`控制输出顺序的情况下。`pthread_create`在循环中，几乎瞬间创建5个线程，但只有第1个线程有机会输出（或者第2个也有，也可能没有，取决于内核调度）如果第3个线程执行了`exit`，将整个进程退出了，所以全部线程退出了。
所以，多线程环境中，应尽量少用，或者不使用exit 函数，取而代之使用`pthread_exit` 函数，将单个线程退出。任何线程里exit 导致进程退出，其他线程未工作结束，主控线程退出时不能return或exit。
另注意，`pthread_exit`.或者`return`返回的指针所指向的内存单元必须是全局的或者是用`malloc`分配的，不能在线程函数的栈上分配，因为当其它线程得到这个返回指针时线程函数已经退出了。

### pthread_join函数

阻塞等待线程退出，获取线程退出状态   起作用，对应进程 wait()函数

```c
#include <pthread.h>
int pthread_join(pthread_t thread, void **retval);
// 参数：
//   thread 线程id
//   retval 传出参数 线程退出状态

// 返回值：
//   成功：0
//   失败：错误号
```

example

```c
struct tval{
  int ret;
  char *str;
};

void sys_err(char *str)
{
  perror(str);
  exit(1);
}

void* func()
{
  struct tval* ret;
  ret = (struct tval*)malloc(sizeof(struct tval));
  ret->ret = 0;
  ret->str = "hellow word";

  return ret;
}

int main(int argc,char *argv[])
{
  pthread_t tid;
  int ret;
  struct tval* retval;
  ret =  pthread_create(&tid, NULL, func, NULL);
  if(ret != 0){
    sys_err("pthread_create error");
  }
  ret = pthread_join(tid, (void**)&retval);
  printf("retval->ret = %d,\nretval-> str = %s\n",retval->ret, retval->str);
  return 0;
}
```

### pthread_cancel函数

杀死线程（线程必须在取消点，进入内核）  作用对应进程中的kill函数

```c
#include <pthread.h>
int pthread_cancel(pthread_t thread);   //被杀死的线程返回-1
// 参数：
//   thread 带杀死的线程id
// 返回值：
//   成功 0
//   失败 错误号
// 如果，子线程没有到达取消点，那么pthread_cance1无效。
// 我们可以在程序中，手动添加一个取消点。使用pthread_testcance1();
// 成功被pthread_cance1()杀死的线程，返回-1 使用pthead_join回收。

void pthread_testcancel(void);

```

```c
void *tfn(void *arg)
{
  while(1){
  // printf("i= %ld, thread: pid = %d, tid = %lu\n",(intptr_t)arg, getpid(), pthread_self());
  // sleep(1);
  pthread_testcancel();   //添加取消点
  }
}

int main(int argc,char *argv[])
{
  pthread_t tid;
  int i, ret; 
  ret = pthread_create(&tid, NULL, tfn, (void *)(intptr_t)i);  //传参采用值传递
  if(ret != 0){
    perror("pthread_create err");
  }
  sleep(5);
  pthread_cancel(tid);  //终止线程
  return 0;
}
```

### pthread_detach函数

实现线程分离

```c
#include <pthread.h>
int pthread_detach(pthread_t thread);
// 参数：
//   thread 要分离的线程id

// 返回值
//   成功 0
//   失败 错误号

// 线程分离状态:指定该状态，线程主动与主控线程断开关系。线程结束后，其退出状态不由其他线程获取，而直接自己自动释放。网络、多线程服务器常用。
// 进程若有该机制，将不会产生僵尸进程。僵尸进程的产生主要由于进程死后，大部分资源被释放，一点残留资源仍存于系统中，导致内核认为该进程仍存在。
// 也可使用pthread_create函数参2(线程属性)来设置线程分离。

```

一般情况下，线程终止后，其终止状态一直保留到其它线程调用pthread_join获取它的状态为止。但是线程也可以被置为detach状态，这样的线程一旦终止就立刻回收它占用的所有资源，而不保留终止状态。
不能对一个已经处于detach状态的线程调用pthread_join，这样的调用将返回EINVAL错误。也就是说，如果已经对一个线程调用了 pthread _detach就不能再调用pthread_join 了。

```c
void *func(void* arg)
{
  printf("thread pid = %d thread id= %ld\n",getpid(), pthread_self());
}
int main(int argc,char *argv[])
{
  pthread_t tid;
  int ret;
  ret = pthread_create(&tid, NULL, func, NULL);
  if(ret != 0){
    fprintf(stderr,"pthread_create error : %s\n", strerror(ret));
  }
  ret = pthread_detach(tid); //设置线程分离
  if(ret != 0){
    fprintf(stderr,"pthread_create error : %s\n", strerror(ret));
  }
  ret = pthread_join(tid, NULL);
  printf("join ret = %d\n",ret);
  if(ret != 0){
    fprintf(stderr, "pthread_create error : %s\n", strerror(ret));
  }
  pthread_exit(NULL);
}

```

## 线程同步(锁)

同步即协同步调，按预定的先后次序运行。
线程同步，指一个线程发出某一功能调用时，在没有得到结果之前，该调用不返回。同时其它线程为保证数据一致性,不能调用该功能。

### 互斥锁(mutex)

建议锁!对公共数据进行保护。所有线程【应该】在访问公共数据前先拿锁再访问。但，锁本身不具备强制性。
使用mutex(互斥量，互斥锁)一般步骤

  1. pthread_mutex_t lock 创建锁
  2. pthread_mutex_init() 初始化
  3. pthread_mautex_lock() 加锁
  4. 访问共享数据(stdout)
  5. pthrad_mmtext_un1ock()  解锁
  6. pthead_mutex_destroy()  销毁锁

初始化

  1. pthread_mutex_init(&mutex, NULL);  //动态初始化
  2. mutex = PTHREAD_MUTEX_INITIALIZER;  //静态初始化

注意事项:
尽量保证锁的粒度，越小越好 (访问共享数据前，加锁。访问结束【立即】解锁。)
(mutex)互斥锁，本质是结构体。我们可以看成整数。初值为1。(pthread_mutex_init()函数调用成功。)
加锁。--操作    阻塞线程
解锁 :++操作    唤醒阻塞在锁上的线程
try锁 尝试加锁 成功--   失败 返回  同时设置错误号 EBUSY

```c
restrict 关键字 :
  用来限定指针变量 被关键字限定的指针变量所指向的内存操作，必须由本指针操作
```

lock 与trylock:
lock加锁失败会阻塞，等待锁释放。
trylock.加锁失败直接返回错误号 (如:EBUSY)，不阻塞。

```c
//函数原型
#include <pthread.h>
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);  //初始化互斥锁
// 参数：
//   mutex 锁
//   mutexattr 锁的特性 一般传NULL
// 返回值：
//   成功 0
//   失败 错误号
int pthread_mutex_lock(pthread_mutex_t *mutex);   // 加锁
// 参数：
//   mutex 锁
// 返回值：
//   成功 0
//   失败 错误号
int pthread_mutex_trylock(pthread_mutex_t *mutex);   //加锁 非阻塞
// 参数：
//   mutex 锁
//   mutexattr 锁的特性 一般传NULL
// 返回值：
//   成功 0
//   失败 错误号
int pthread_mutex_unlock(pthread_mutex_t *mutex);   //释放互斥锁
// 参数：
//   mutex 锁
//   mutexattr 锁的特性 一般传NULL
// 返回值：
//   成功 0
//   失败 错误号
int pthread_mutex_destroy(pthread_mutex_t *mutex);   //销毁互斥锁，被销毁的锁要从新初始化
// 参数：
//   mutex 锁
//   mutexattr 锁的特性 一般传NULL
// 返回值：
//   成功 0
//   失败 错误号


```

### 死锁

使锁不恰当使用的

1. 反复加锁

### 读写锁

于互斥量类似，但读写锁允许更高的并行性。其特性为  

1. 写独占，读共享  
2. 写锁优先级高
相较于互斥量而言，当读戮程多的时候，提高访问效率

**读写锁状态**
特别强调：读写锁只有一把，但其具备两种状态

1. 读模式下加锁状态（读锁）
2. 写模式下加锁状态（写锁）

#### 读写锁函数

```c
#include <pthread.h>
pthread_rwlock_t 类型用于定义一个读写锁变量。
pthread_rwlock_t rwlock;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER; //静态初始化
int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, const pthread_rwlockattr_t *restrict attr); 
动态初始化
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);   //销毁读写锁
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);    //以写方式锁
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);    //以读方式锁
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);  //非阻塞
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);    //解锁

上7个函数的返回值都是。成功返回o，失败直接返回错误号。

pthread_rwlock_t 类型用于定义一个读写锁变量。
pthread_rwlock_t rwlock;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER; //静态初始化

```

### 条件变量

本身不是锁，但它也可以造成线程阻塞。通常与互斥锁配合使用。给多线程提供一个会合的场所。

```c
//主要应用函数
pthread_cond_init()    //初始化
pthread_cond_destroy()  //删除
pthread_cond_wait()   //等待
pthread_cond_timedwait()  //设置等待超时时间
pthread_cond_signal()   //发出通知
pthread_cond_broadcast()  //广播通知

//以上6个函数的返回值都是:成功返回0，失败直接返回错误号。
pthread_cond_t 类型用于定义条件变量。 
pthread_cond_t cond;
//初始化
  pthread_cond_init(&cond, NULL);  //动态初始化
  cond = PTHREAD_COND_INITIALIZER;  //静态初始化

```

```c
//函数原型
#include <pthread.h>
int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *cond_attr);  // 动态初始化条件变量
参数：

返回值
  成功 0
  失败 错误号
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex); //阻塞等待一个条件变量
// 函数作用
// 1. 阻塞等待条件变量 cond满足
// 2. 释放已经掌握的互斥锁（解锁互斥量） 相当于pthread_mutex_unlock();
//   **1、2 步为一个原子操作**
// 3. 当被唤醒，pthread cond_walt()函数返回时，解除阻塞并重新申请获取互斥锁 pthread mutex lock(&mutex);

int pthread_cond_signal(pthread_cond_t *cond);  
//唤醒至少一个阻塞在条件变量上的一个线程

int pthread_cond_broadcast(pthread_cond_t *cond);  
//唤醒所有阻塞在条件变量上的线程

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
//阻塞等待条件变量 ，但设置超时

int pthread_cond_destroy(pthread_cond_t *cond);

```

#### 生产者消费者模型

要求能够借助条件变量完成生产者消费者模型
![生产者消费者模型](Linux_system_program.assets/Producer_consumer_model.png)

```c
void sys_err(char *str, int _errno)
{
  fprintf(stderr, "%s :%s\n", str, strerror(_errno));
}

struct msg{
  int num;
  struct msg* next;
};
struct msg* head;

void* product(void* arg);
void* consumer(void* arg);


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int main(int argc,char *argv[])
{
  int ret;
  pthread_t pid,cid;
  srand(time(NULL));
  ret = pthread_create(&pid, NULL, product, NULL);  //生产者
  if(ret != 0){
    sys_err("pthread_create err", ret);
  }
  pthread_create(&cid, NULL, consumer, NULL);    //消费者
  if(ret != 0){
    sys_err("pthread_create err", ret);
  }

  pthread_create(&cid, NULL, consumer, NULL);    //消费者
  if(ret != 0){
    sys_err("pthread_create err", ret);
  }

  pthread_create(&cid, NULL, consumer, NULL);    //消费者
  if(ret != 0){
    sys_err("pthread_create err", ret);
  }

  pthread_join(pid, NULL);
  if(ret != 0){
    sys_err("pthread_join err", ret);
  }
  pthread_join(cid, NULL);
  if(ret != 0){
    sys_err("pthread_join err", ret);
  }


  return 0;
}

void* product(void* arg){   //生产者

  while(1){
    
    struct msg *mp = (struct msg*)malloc(sizeof(struct msg));
    mp->num = rand() % 1000;         //模拟生产数据
    pthread_mutex_lock(&mutex);
    mp->next = head;                 //写公共区域
    head = mp;
    
    pthread_mutex_unlock(&mutex);
    printf("product %d------\n", mp->num);
    pthread_cond_signal(&cond);    //唤醒阻塞在条件变量的线程 生产者唤醒消费者
    
    sleep(rand() % 3);
  }

  return NULL;
}

void* consumer(void* arg){  //消费者
  while(1){
    pthread_mutex_lock(&mutex);
    while(head == NULL){         // 检查条件变量
      pthread_cond_wait(&cond, &mutex);   //阻塞等待条件边量   阻塞过程中解锁，  阻塞结束后加锁
    }
     struct msg *mp = head;
     head = mp->next;

    pthread_mutex_unlock(&mutex);
    printf("------consumer id = %lu : %d\n",pthread_self(), mp->num);
    free(mp);
    sleep(rand() % 3);

  }

  return NULL;
}

```

### 信号量（semaphore）

相当于初始化值为 N 的互斥量  N值表示可以同时访问共享数据区的线程数
可以应用于进程与线程

```c
#include <semaphore.h>
//主要应用函数
  sem_init()  // 初始化信号量
  sem_destroy()  // 销毁信号量
  sem_wait()   //给信号量加锁  信号量--
  sem_trywait() //非阻塞
  sem_timewait()  //定时
  sem_post()  // 给信号量解锁，并发送信号  信号量++
```

```c

// 函数原型

int sem_init(sem_t *sem, int pshared, unsigned int value);   
// 参数：
//   sem： 信号量
//   pshare： 0：线程间同步   1：进程间同步
//   value：指定同时访问的线程数（N值）
// 返回值：
//   成功 0
//   失败 -1 errno

int sem_destroy(sem_t *sem);
// 参数：
//  sem： 信号量
// 返回值：
//   成功 0
//   失败 -1 errno

int sem_post(sem_t *sem);  //sem_t ++
// 参数：
//  sem： 信号量
// 返回值：
//   成功 0
//   失败 -1 errno

int sem_wait(sem_t *sem); //sem_t --
// 参数：
//  sem： 信号量
// 返回值：
//   成功 0
//   失败 -1 errno

int sem_trywait(sem_t *sem);
// 参数：
//  sem： 信号量
// 返回值：
//   成功 0
//   失败 -1 errno


int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
struct timespec {
    time_t tv_sec;      /* Seconds */
    long   tv_nsec;     /* Nanoseconds [0 .. 999999999] */   纳秒
};
// 参数：
//  sem： 信号量
//  abs_timeout：最大阻塞时间(绝对时间)

// 返回值：
//   成功 0
//   失败 -1 errno

//定时一秒
  time_t cur = time(NULL);  //获取当前时间
  struct timespec t; //定义时间结构体变量
  t.tv_sec = cur + 1; //定时一秒
  sem_timedwait(&sem, &t); 


```

#### 信号量基本操作

```c
sem_wait()     // 1. 信号量大于0 则信号量--    (类比pthread_mutex_lock)
   |           // 2. 信号量等于0 则线程阻塞
  对应
   |
sem_post()     // 将信号量++ 同时隐藏阻塞在信号量上的线程  （类比pthread_mutex_unlock）

// 由于sem_t的实现对用户隐藏，所以所谓的++、--操作只能通过函数来实现，而不能直接++、--符号
// 信号量的初值，决定了占用信号量的线程个数

```

#### 信号量实现生产者消费者模型

![信号量实现生产者消费者模型](Linux_system_program.assets/sem_product_consumer.png)

```c
void sys_err(char *str, int _errno)
{
  fprintf(stderr, "%s :%s\n", str, strerror(_errno));
}

void* consumer(void* arg);   //消费者
void* product(void* arg);   //生产者

#define NUM 5
int sem_quene[NUM];
sem_t blank_number, product_number;   //队列空白信号量数量  产品信号量数量


int main(int argc,char *argv[])
{
  srand(time(NULL));  //随机数种子
  pthread_t cid, pid;
  sem_init(&blank_number, 0, 5);
  sem_init(&product_number, 0, 0);  // 信号量队列初始化
  pthread_create(&pid, NULL, product, NULL);
  pthread_create(&cid, NULL, consumer, NULL);
  pthread_join(pid, NULL);   //线程回收
  pthread_join(cid, NULL);
  sem_destroy(&blank_number);   //信号量回收
  sem_destroy(&product_number);
  pthread_exit(NULL);
}



void* product(void* arg)   //生产者
{   
  int i = 0;

  while(1){
    sem_wait(&blank_number);


    sem_quene[i] = rand() % 1000;   //临界区
    printf("----product %d\n",sem_quene[i]);


    sem_post(&product_number);    //将产品数++
    i = (i + 1) % NUM;    //循环队列
    sleep(1);
    sleep(rand() % 1);
  }

  return NULL;
}

void* consumer(void* arg)   //消费者
{  
  int i = 0;

  while(1){
    sem_wait(&product_number);

    printf("-Consume--- %d\n",sem_quene[i]);  //临界区
    sem_quene[i] = 0;

    sem_post(&blank_number);   //空格数++
    i = (i+1) % NUM;
    sleep(rand() % 3);

  }
  return NULL;
}
```
