[TOC]

# linux基础

## 根目录介绍

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

## cd用法（change directory）

-
  cd       切换目录

- cd ~     切换到家目录
- cd /etc  切换到etc目录
- cd -     回到上一次操作的路径
- cd ..    回到上级目录

## 文件操作

```shell
ls（list）  #作用：列出当前目录下的文件
#用法：
ls  #查看当前目录
ls -l #列出当前目录下的文件详细信息
ls -a #查看当前目录所有文件
ls -lh #带单位显示文件大小
#.（点） 当前目录
#..（点点） 上一级目录
#../../ 上上级目录

touch   #作用：创建普通文件、更新文件时间
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
rm filename  #删除文件
rm dirname -r #删除目录，删除目录及子目录

rmdir  #作用：删除空目录
#用法：
rmdir <directory>

cp（copy）    #作用：复制文件或目录
#用法：
cp file1 file2    #文件到文件，如果没有file2文件，则创建文件
cp file dir     #文件到目录（操作文件到目录里面去不用加-r，本质是操作文件）
cp dir1/ dir2 -r  #目录复制到目录
#注意：对目录的操作要加 -r 选项   递归作用
#-f   强制操作
chmod     #修改文件权限
用法：
chmod  <八进制数>  <filename>
chmod 777 3.txt  #修改3.txt拥有所有权限
chmod u+x 3.txt  #给3.txt的user用户添加执行权限
chmod g+w 3.txt  #给3.txt的group组添加写权限
chmod o+r 3.txt  #给3.txt的other其他人添加读权限
#r ：读权限  w：写权限  x：可执行权限
u（user）#文件所属用户
g（group） # 文件所属用户组
o（other） #其他人
八进制数表示 :  r  w  x
     4 2 1

```

## linux文件类型

```shell
-  #普通文件
d  #目录文件
c  #字符设备文件
b  #块设备文件
l  #软连接
p  #管道文件
s  #套接字
#其他未知文件
```

## linux系统目录

- bin：存放二进制文件
- boot：存放开机启动程序
- dev：存放设备文件 ：字符设备，鼠标键盘
- home：存放普通用户
- etc：用户信息和系统配置文件
- lib：库文件
- root：管路员宿主目录（家目录）
- usr：用户资源管路目录

## 文件系统

- inode
  - 本质为结构体，存储文件的存储信息 如：权限，类型，大小，时间，用户，盘块位置，大多数的inode都存储在磁盘上

- dentry
  - 目录项，本质依然是结构体，重要的成员变量有两个，{文件名，inode...},而文件内容保存在磁盘盘块中

## 基本命令

- echo $SHELL  #查看当前命令解析器
- which    #查看指定命令所在路径
- pwd      #查看当前目录
- wc    #计算文件的Byte数，字数或列数
- od    #指定数据的显示格式
- du    #显示磁盘大小
- df    #查看磁盘使用情况

## 用户管理

```shell
whoami   #查看当前登录用户
sudo adduser <用户名>     #创建用户 
sudo addgroup <用户组名>  #创建用户组
sudo chown <新用户名>   <文件名> #修改文件所属用户
sudo chgrp <用户组名>  <文件名>   #修改文件所属用户组
```

## 查找与检索

```shell
stat   #查看文件的状态（修改时间，大小，权限。。。）
find     #查找文件
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

## 压缩与解压

```shell
#gzip命令压缩  gizp压缩只会压缩目录下的文件
gzip <-r> <failename> #压缩
gzip -d <failename>  #解压

#tar命令压缩解压
tar -zcvf <包名> <源文件>。。。   #以gzip方式压缩
tar -zxvf <包名>   #以gzip方式解压缩

tar -jcvf <包名> <源文件>。。。   #以bzip2方式压缩
tar -jxvf <包名>   #以bzip2方式解压缩

选项：（可以结合起来使用，但是f需要放在最后一位，例如-cvzf）
-t	显示归档文件中的内容
-x	释放归档文件
-u	更新归档文件
-c	创建一个新的归档文件
-v	显示归档和释放的过程信息
-f	用户指定归档文件的文件名，否则使用默认名称
-j	由tar生成归档，然后由bzip2压缩
-z	由tar生成归档，然后由gzip压缩 
```

## vim

```shell
vi/vim:以vi编辑器的形式打开一个文件；
vi 文件名 +n：打开文件，并且让光标移动到第n行；
如果文件不存在就创建；
命令行：
gg:回到首行
G:让光标移动到末尾
gg=G:自动缩进；
在可视模式下选中文本点击=：自动缩进；
ZZ:保存并退出；
底行：
set mouse=a：开启鼠标使能；
set mouse-=a：关闭鼠标使能；
set nu：开启行号
set nonu：关闭行号
vsp：在当前终端横向额外打开一个文件；
```

## 软件包

### dpkg

```shell
离线安装软件；
在最前面加上sudo：临时获取超级用户权限；
    -i      install安装
    -r      remove卸载
    -p		删除软件的配置

例子：
	sudo dpkg -i oneko_1.2.sakura.6-14_amd64
```

### apt

```shell
apt-get install 安装的软件名
在线安装一个软件；
apt-get remove 卸载软件
apt-get clean  清除所以软件包
apt-get update 更新软件源

sudo apt-get install 软件名
sudo apt-get install -f 软件名 强制安装
sudo apt-get install sl 跑火车；
sudo apt-get install bastet 安装俄罗斯方块
sudo apt-get install oneko 追鼠
sudo apt-get install cmatrix 代码雨；

通过apt-get下载的软件，软件包会放在以下路径：
/var/cache/apt/archives
```



## gcc

```shell
-I    #制定头文件所在的目录
-c   #只做预处理，编译，汇编，得到二进制文件
-g   #编译时添加调试文件，主要用于gdb调试
-On n=0~3 #编译优化，n越大，优化越多
-Wall  #显示所有警告信息
-D   #在程序中注册一个宏
```

## 静态库

```shell
ar rcs libmylib.a file.o  #将file.o加入到libmyllib.a中

#静态库制作步骤,及使用
1.将.c文件用gcc -c命令生成.o文件
2.使用ar命令加入静态库文件
  ar rcs <静态库文件名> <编译好的.o文件>...
3.编译静态库文件到可执行文件中
  gcc test.c lib<库名>.a -o a.out
```

## 动态库

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

## gdb

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

## makefile项目管理

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