[TOC]

### linux基础

##### 根目录介绍

- bin  存放二进制可执行文件
- boot  存放开机启动程序
- dev  存放设备文件
- etc  用户信息和系统配置
- home  用户目录
- lib   库路径
- media/mnt   磁盘挂载相关
- opt proc
- root  root用户目录
- usr  用户资源管理目录 



##### cd用法（change directory）

- 
  cd       切换目录

- cd ~     切换到家目录
- cd /etc  切换到etc目录
- cd -     回到上一次操作的路径
- cd ..    回到上级目录


##### 文件操作

```shell
ls（list）  #作用：列出当前目录下的文件
#用法：
ls		#查看当前目录
ls -l	#列出当前目录下的文件详细信息
ls -a	#查看当前目录所有文件
ls -lh	#带单位显示文件大小
#.（点）	当前目录
#..（点点）	上一级目录
#../../	上上级目录

touch  	#作用：创建普通文件、更新文件时间
#用法：
touch <filename> #如果filename存在，更新文件时间
			   #如果不存在，就创建文件

file  #作用：查看文件属性
#用法：
file <filename>

cat#作用：查看文件内容，不打开文件
#用法：
cat <filename>
cat <filename> -n：#带行号

#其他查看命令
tac <filename> #按行倒序查看内容
more <filename> #分屏查看内容
less <filename> #分屏查看内容（不显示行号）
head <-num> <filename> #显示前num行,默认10行
tail <-num> <filename> #显示后num行,默认10行
tree #显示当前文件树状结构

mkdir（make dierctory）#作用：创建目录
#用法：
mkdir dirname：#创建一个目录

rm（remove） #作用：删除文件或目录
#用法：
rm filename 	#删除文件
rm dirname -r	#删除目录，删除目录及子目录

rmdir  #作用：删除空目录
#用法：
rmdir <directory>

cp（copy）	   #作用：复制文件或目录
#用法：
cp file1 file2    #文件到文件，如果没有file2文件，则创建文件
cp file dir     #文件到目录（操作文件到目录里面去不用加-r，本质是操作文件）
cp dir1/ dir2 -r  #目录复制到目录
#注意：对目录的操作要加 -r 选项   递归作用
#-f   强制操作
chmod     #修改文件权限
用法：
chmod  <八进制数>  <filename>
chmod 777 3.txt 	#修改3.txt拥有所有权限
chmod u+x 3.txt 	#给3.txt的user用户添加执行权限
chmod g+w 3.txt 	#给3.txt的group组添加写权限
chmod o+r 3.txt 	#给3.txt的other其他人添加读权限
#r ：读权限  w：写权限  x：可执行权限
u（user）#文件所属用户
g（group） # 文件所属用户组
o（other） #其他人
八进制数表示 :  r  w  x
			  4 2 1

```



##### linux文件类型

```shell
f  #普通文件
d  #目录文件
c  #字符设备文件
b  #块设备文件
l  #软连接
p  #管道文件
s  #套接字
#其他未知文件
```



##### linux系统目录

* bin：存放二进制文件
* boot：存放开机启动程序
* dev：存放设备文件 ：字符设备，鼠标键盘
* home：存放普通用户
* etc：用户信息和系统配置文件
* lib：库文件
* root：管路员宿主目录（家目录）
* usr：用户资源管路目录



##### 文件系统

* inode
  * 本质为结构体，存储文件的存储信息 如：权限，类型，大小，时间，用户，盘块位置，大多数的inode都存储在磁盘上

* dentry 
	* 目录项，本质依然是结构体，重要的成员变量有两个，{文件名，inode...},而文件内容保存在磁盘盘块中
	

##### 基本命令

* echo $SHELL		#查看当前命令解析器
* which 			#查看指定命令所在路径
* pwd   			#查看当前目录
* wc				#计算文件的Byte数，字数或列数
* od				#指定数据的显示格式
* du				#显示磁盘大小
* df				#查看磁盘使用情况



##### 用户管理

```shell
whoami   #查看当前登录用户
sudo adduser <用户名>     #创建用户 
sudo addgroup <用户组名>  #创建用户组
sudo chown <新用户名>   <文件名> #修改文件所属用户
sudo chgrp <用户组名>  <文件名>   #修改文件所属用户组
```

##### 查找与检索

```shell
stat   #查看文件的状态（修改时间，大小，权限。。。）
find    	#查找文件
用法：
find  <路径目录>  <-maxdepth>  <搜索类型> <搜索字符串> ...  
#搜索类型： -name 按名字     -type 按文件类型  -size 按文件大小 -maxdepth 指定搜索深度
example:
find <路径目录>  -size <+20M> -size <-50M>   #查找文件大小>20 <50的文件
find ./ -type 'f' -maxdepth 1 -exec(运行命令) ls -l {} \;   #将前面得到的结果集合进行ls操作 exec执行后面的操作
find ./ -type 'f' -maxdepth 1 -ok rm -r         {} \;    #将前面得到的结果集合进行rm操作并询问是否执行操作

grep #查找文件内容或筛选结果集
grep -r 'copy' ./ -n
     -r递归查找     -n  显示行号

ps aux | grep 'cupsd'   #检索进程结果集
```



##### 压缩与解压 

```shell
#gzip命令压缩  gizp压缩只会压缩目录下的文件
gzip <-r> <failename> #压缩
gzip -d <failename>  #解压

#tar命令压缩解压
tar -zcvf <压缩包名> <压缩源文件>。。。   #以gzip方式压缩
tar -zxvf <压缩包名>   #以gzip方式解压缩

tar -jcvf <压缩包名> <压缩源文件>。。。   #以bzip2方式压缩
tar -jxvf <压缩包名>   #以bzip2方式解压缩

```

##### gcc

```shell
-I 			#制定头文件所在的目录
-c			#只做预处理，编译，汇编，得到二进制文件
-g			#编译时添加调试文件，主要用于gdb调试
-On n=0~3	#编译优化，n越大，优化越多
-Wall		#显示所有警告信息
-D			#在程序中注册一个宏
```

##### 静态库

```shell
ar rcs libmylib.a file.o  #将file.o加入到libmyllib.a中

#静态库制作步骤,及使用
1.将.c文件用gcc -c命令生成.o文件
2.使用ar命令加入静态库文件
  ar rcs <静态库文件名> <编译好的.o文件>...
3.编译静态库文件到可执行文件中
  gcc test.c lib<库名>.a -o a.out
```

##### 动态库

```shell
#动态库的制作及使用
1.将.c文件生成.o文件 (生成与位置无关的代码 -fPIC)
  gcc -c add.c -o add.o -fPIC
2.使用gcc -shared 制作动态库
  gcc -shared <-o> lib<库名>.so <编译好的.o文件>...
  #如果没有库文件用-o创建
3.编译可执行程序时指定使用的动态库。 -l 指定库名  -L指定库路径
  gcc test.c -o a.out -l<mymath> -L<./lib>
4.运行可执行程序  ./out （会出错）
    #原因：
      #链接器：工作于链接阶段，需要-l -L
      #动态链接器：工作于运行阶段，工作时需要提供动态库所在目录
    #解决：
    #通过环境变量： export LD_LIBRARY_PATH=<动态库路径> 临时生效
    #要使永久生效（写入终端配置文件）
      #1.vim ~/.zshrc
      #2.写入 export LD_LIBRARY_PATH=<动态库路径>
      #3.source .zshrc / 重启终端 ->使配置生效
    #滥竽充数法：拷贝动态库到/lib下
    #配置文件法
      #1.sudo vi /etc/ld.so.config
      #2.写入动态库绝对路径 保存
      #3.sudo ldconfig -v 使配置文件生效


```
##### gdb

```shell
gcc -g <文件名> -o a.out 使用-g参数编译可执行文件，得到调试表
gdb a.out  #开始调试

list：list <1>/ l <1> #列出源码，
break： break 20 / b 20  #在20行位置设置断点
run/r   #运行程序
n/next  #下一条指令
s/step  #下一条指令（会进入函数）
p/print #查看变量的值
continue  #继续执行断点后续指令
quit #退出

段错误排查
直接gdb run

其他命令介绍



```

##### makefile项目管理

```shell
命名：makefile Makefile
1个规则
  目标：依赖条件
          （一个tab缩进）
  目标的时间必须晚于依赖的时间，否则更新目录
  依赖条件不存在，找寻新的规则去产生为依赖

2个函数
src = $(wildcard *.c)   $(wildcard *.c)
#找到当前目录下所有后缀为.c的文件，赋值给src
obj(可修改名称) = $(patssubst %.c, %.o, $(src))
#把src变量里所有后缀为.c的文件替换为.o赋值给obj

clean: #（没有依赖）
    -rm -rf $(obj)  #"第一个-"作用是删除不存在的文件时不报错

3个自动变量
$@  在规则的命令中表示规则的目标
$^  在规则的命令中表示所有依赖条件
$<  在规则的命令中表示第一个依赖条件。 如果将变量应用于模式规则中，他可将依赖条件中的依赖依次取出，套用模式规则

模式规则
%.o:%.c
  gcc -c $< -o $@

静态模式规则
$(obj):%.o:%.c
  gcc -c $< -o $@
%.o:%.s
  gcc -S $< -o $@

伪目标
.PHONY: clean ALL

参数
  -n 模拟执行make clean命令
  -f 指定makefile文件执行
```

### 文件I/O

```c
open #include <unistd.h>所包含
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
//flag常用参数(使用头文件fcntl.h)
O_RDONLY  O_WRONLY  O_RDWR #读 写 读写权限 
O_APPEND O_CREAT O_EXCL O_TRUNC O_NONBLOCK
//创建文件时，指定文件访问权限。权限同时受umask影响。结论为
//文件权限 = mod & ~umask

//常见错误
//1.打开文件不存在
//2.以写方式打开只读文件
//3.以只写方式打开目录

错误处理函数 与errno相关

```


##### read write
```c
ssize_t read(int fd,void *buf,size_t count)
```
参数（头文件 unistd.h）
fd 文件描述符
buf 存数据的缓冲区
count 缓冲区的大小
返回值
成功 读到的字节数
失败 -1，设置errno
如果 -1，并且 errno = EAGIN 或 EWOULDBLOCK吗，说明不是read失败，而是read在以非阻塞方式读设备文件或网络文件  

```c
ssize_t write(int fd,const void *buf,size_t count)
```
参数（头文件 unistd.h）
fd 文件描述符
buf 待写入数据的缓冲区
count 写入数据的大小
返回值
成功 写入的字节数
失败 -1，设置errno
```c
/*
 *./mycp src dst 命令行参数实现简单的cp命令
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

##### 文件描述符
```
PCB(process control block) 进程控制块  本质：结构体
成员：文件描述符
文件描述符：0-1023    文件描述符使用表中可用的最小的
0 - STDIN_FILENO
1 - STDOUT_FILENO
3 - STDERR_FILENO
```
##### 阻塞，非阻塞  
```
阻塞，非阻塞  是设备文件，网络文件的属性
	产生阻塞的场景。 读设备文件，读网络文件。(读常规文件无阻塞概念)
	/dev/tty

```


##### fcntl 改打开文件属性
```c
int flags = fcntl(fd, F_GETFL);
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



##### lseek函数
```c
off_t lseek(int fd, off_t offset, int whence);
```
参数
  fd：文件描述符
  offset：偏移量
  whence：起始偏移位置： SEEK_SET/SEEK_CUR/SEEK_END(文件 起始位置/当前位置/末尾位置)
返回值
  成功：较起始位置偏移量
  失败：-1 errno

应用场景：文件'读''写'使用同一偏移位置
        使用lseek获取文件大小
        使用lseek拓展文件大小
          可以使用truncate 函数直接拓展文件 int ret = truncate("dict.cp",250); 将文件dict.cp拓展到250字节

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


##### ioctl函数
对设备的I/O通道进行管理，控制设备特性

##### 传入传出参数
* 传入参数
1. 指针作为函数参数
2. 通常由const关键字修饰
3. 指针在有效区，在函数内部做读参数

* 传出参数
1. 指针作为函数参数
2. 在函数调用之前指针指向的空间可以无意义，但必须有效
3. 在函数内部做写操作
4. 函数调用结束后充当函数返回值

* 传入传出函数
1. 指针做函数参数
2. 在函数调用前，指针指向的空间有实际意义
3. 在函数内部，先做读操作，再做写操作
4. 函数调用中，充当函数返回值

例： 
```c
char *strcpy(char *dest, const char *src);
//char *desk为传出参数   const char *src为传入参数

```

##### stat与lstat函数 
获取文件属性(从inode结构体中获取)
```c
int stat(const char *path, struct stat *buf);
int lstat(const char *path, struct stat *buf);
```
参数：

path: 文件路径

buf: (传出参数) 存放文件属性

返回值: 
		成功 0
		失败 -1 errno 

区别：穿透符号链接：stat：会  lstat：不会

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
  S_ISLNK (st_mode)  判断是否为符号连接
  S_ISREG (st_mode)  是否为一般文件
  S_ISDIR (st_mode)  是否为目录
  S_ISCHR (st_mode)  是否为字符装置文件
  S_ISBLK (s3e)    是否为先进先出
  S_ISSOCK (st_mode)  是否为socket

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



##### link和ulink函数

```c
int link(const char* oldpath, const char* newpath); //创建硬链接
int ulink(const char *pathname);  //删除dentry
```
返回值

成功 0

失败 -1 设置errno

使用link ulink函数实现mv命令
```c
int main(int argc,char *argv[])
{
    link(argv[1],argv[2]);    //建立argv[1]的硬链接到argv[2]
    ulink(argv[1]);					//删除argv[1]的dentry
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

	ret = unlink("temp.txt");		 //具备了被释放的条件
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

##### getcwd和chdir函数

##### 文件和目录权限 
打开文件夹需要x权限

##### 目录操作函数 opendir、closedir、readdir
```c
//#include <dirent.h>
//#include <sys/types.h>
DIR *opendir(const char* name); //打开文件夹  成功返回DIR*  失败返回NULL
int closedir(DIR *dirp);  //关闭文件夹 
struct dirent *readdir(DIR* dirp); //读文件夹  获取目录     失败返回NULL
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
    if (!strcmp(sdp->d_name, ".") | !strcmp(sdp->d_name, ".."))
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

##### dup(重定向)
```c
int dup(int oldfd);  //返回值为新的文件描述符
int dup2(int oldfd,int newfd); //dupto 使newfd指向oldfd指向的文件 newfd->oldfd oldfd必须为有效的文件描述符 返回值为newfd
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

### 进程控制
程序：死的，只占用磁盘空间
进程：活得，运行起来的程序。占用内存，cpu等系统资源

##### 虚拟内存和物理内存的映射关系
```https://www.bilibili.com/video/BV1KE411q7ee?p=77&spm_id_from=pageDriver&vd_source=a1edf3ad63e77272acbd0c1f89c365d6```

##### pcb进程控制块
每个进程在内核中都有一个进程控制块（PCB）来维护进程相关信息，Linux内核的进程控制块是task_struct结构体
/usr/src/linux-headers-3.16.0-30/include/linux/sched.h文件中可以查看struct task_struct结构体定义。其内部成员有很多，我们重点掌握以下部分即可:
* 进程id (PID) 命令：ps aux可查看
* 进程的状态 （（初始态）， 就绪，运行，挂起，终止等状态）
* 进程切换时需要保存和恢复的一些cpu寄存器
* 描述虚拟地址空间的信息
* 当前工作目录
* *umask掩码
* 文件描述符表
* 和信号相关的信息
* 用户id和组id
* 会话（session）和进程组
* 进程可使用的资源上限（Resource Limit）

##### 环境变量
* PATH
* SHELL 查看当前shell
* TERM  查看当前终端
* LANG  查看当前语言
* HOME  查看当前用户家目录

##### fork函数
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

###### getpid & getppid函数
```c
pid_t getpid(void);  //返回当前进程的pid
pid_t getppid(void); //返回父进程的pid
```




###### 循环创建进程
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
###### 进程共享
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

##### exec函数族
fork创建的程序执行的是父进程相同的程序（但又可能执行不同的代码分支），子进程往往要调用一种exec函数以执行另一个程序。当进程调用一种exec函数时，该进程的用户空间代码和数据完全被新程序替换，从新程序的启动开始执行。调用exec并不会创建新进程，所以调用exec前后台该进程的id并未改变。
将当前进程的.text、.data替换为所要加载的程序的.text、.data,然后让进程从新的.text第一天指令开始执行，但进程id不变，换核不换壳
```c  
int execl(const char *pathname, const char *arg, .../* 可写多个参数 */);
int execlp(const char *file, const char *arg, .../* 可写多个参数 */, NULL/* 结束符 */);
int execle(const char *pathname, const char *arg, .../* 可写多个参数 */);
int execv(const char *pathname, char *const argv[]);
int execvp(const char *file, char *const argv[]); 
int execvpe(const char *file, char *const argv[],char *const envp[]);
```
###### execlp函数
加载一个进程，借助PATH环境变量

###### exexl函数
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


###### 孤儿进程
孤儿进程:父进程先于子进程结束，则子进程成为孤儿进程，子进程的父进程成为init进程，称为init进程领养孤儿进程。



###### 僵尸进程
僵尸进程:进程终止，父进程尚未回收，子进程残留资源（PCB）存放于内核中，变成僵尸（zombie）进程。
特别注意，僵尸进程是不能使用kill命令清除掉的。因为 kill命令只是用来终止进程的,而僵尸进程已经终止。
思考!用什么办法可清除掉僵尸进程呢?  杀掉父进程让init进程回收  wait函数回收

##### wait函数
一个进程在终止时会关闭所有文件描述符，释放在用户空间分配的内存，但它的 PCB还保留着，内核在其中保存了一些信息:如果是正常终止则保存着退出状态，如果是异常终止则保存着导致该进程终止的信号是哪个。这个进程的父进程可以调用wait或 waitpid_获取这些信息，然后彻底清除掉这个进程。我们知道一个进程的退出状态可以在 shell中用特殊变量$?查看，因为shell是它的父进程，当它终止时shell 调用wait或waitpid得到它的退出状态同时彻底清除掉这个进程。
父进程调用wait函数可以回收子进程终止信息。该函数有3个功能：
1. 阻塞等待子进程退出
2. 回收子进程残留资源‘
3. 回去子进程结束状态（退出原因）
```c

#include <sys/types.h>
#include <sys/wait.h>
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
  WEXITSTARUS(status); //如上宏为非0 -> 获取进程退出状态

WIFSIGNALED(status); //为非0 -> 进程异常终止
  WTERMSIGN(status);  //如上宏为真-> 取得进程暂停的那个信号

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

##### 进程通信
现用常用的进程间通信方式
1. 管道（使用最简单）
2. 信号（开销最小）
3. 共享映射区（无血缘关系）
4. 本地套接字（最稳定）

###### 管道
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
* 读管道
  * 管道有数据，read返回实际读到的数据
  * 管道无数据
    * 无写端 raed返回0 （类似读到文件尾）
    * 有写端，read阻塞等待
* 写管道
  * 管道读端全部被关闭，进程异常终止（也可以使用捕捉 SIGPIPE 信号，使进程不终止）
    * 管道已满:write阻塞
    * 管道未满，write将数据写入，并返回实际写入的字节数 

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