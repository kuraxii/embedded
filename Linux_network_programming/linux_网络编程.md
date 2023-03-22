[toc]

### linux网络基础

#### 七层模型与四层模型及代表协议

##### OSI七层模型

| OSI参考模型 |      |     TCP/IP模型     |
| :---------: | :--: | :----------------: |
|   应用层    |  ⌉   |                    |
|   表示层    | ---> |       应用层       |
|   会话层    |  ⌋   |                    |
|   传输层    |      |       传输层       |
|   网络层    |      |       网络层       |
| 数据链路层  | ---> | 网络接口层(链路层) |
|   物理层    |  ⌋   |                    |

##### TCP/IP协议代表协议

应用层: http,ftp,nfs,ssh,telnet...
传输层: TCP UDP
网络层: IP ICMP IGMP
链路层: 以太网帧协议, arp协议

##### 网络传输流程

数据没有封装之前，是不能在网络中传递。
**数据**--封装-->**应用层**--封装-->**传输层**--封装-->**网络层**--封装-->**链路层**

##### 以太网帧协议

ARP协议:根据Ip地址获取mac地址。
以太网帧协议:根据mac地址，完成数据包传输。

##### IP协议

版本:IPv4、IPv6 -- 4位
TTL: time to live 。设置数据包在路由节点中的跳转上限。每经过一个路由节点，该值-1，减为o的路由，有义务将该数据包丢弃
源IP:32位。--- 4字节   192.168.1.108---点分十进制IP地址(string) ---二进制
目的IP:32位。--- 4字节

IP地址:可以在网络环境中，唯一标识一台主机。
端口号:可以网络的一台主机上，唯一标识一个进程。
IP地址+端口号:可以在网络环境中，唯一标识一个进程。

#### Socket编程

套接字概念
一个文件描述符指向一个套接字（该套接字内部由内核借助两个缓冲区实现）
在通信过程中，套接字一定使成对出现的

##### 网络字节序

我们已经知道，内存中的多字节数据相对于内存地址有大端和小端之分，磁盘文件中的多字节数据相对于文件中的偏移地址也有大端小端之分。网络数据流同样有大端小端之分，那么如何定义网络数据流的地址呢﹖发送主机通常将发送缓冲区中的数据按内存地址从低到高的顺序发出，接收主机把从网络上接到的字节依次保存在接收缓冲区中，也是按内存地址从低到高的顺序保存，因此，网络数据流的地址应这样规定:先发出的数据是低地址，后发出的数据是高地址。
TCP/IP 协议规定，网络数据流应采用大端字节序，即低地址高字节。例如上一节的UDP段格式，地址0-1是16位的源端口号，如果这个端口号是1000
(Ox3e8)，则地址o是Ox03，地址1是Oxe8，也就是先发Ox03，再发 Oxe8，这16位在发送主机的缓冲区中也应该是低地址存Ox03，高地址存Oxe8。但
是，如果发送主机是小端字节序的，这16位被解释成Oxe803，而不是1000。因此，发送主机把1000填到发送缓冲区之前需要做字节序的转换。同样地，
接收主机如果是小端字节序的，接到16 位的源端口号也要做字节序的转换。如果主机是大端字节序的发送和接收都不需要做转换。同理，32位的P地址也
要考虑网络字节序和主机字节序的问题。

```c
//ip地址网络字节序和主机字节序的转换
#include<arpa/inet.h>
uint32_t htonl(uint32_t hostlong);  //host to net   针对于IP地址
uint16_t htons(uint16_t hostshort); //host to net   针对于port
uint32_t ntohl(uint32_t netlong); //net to host   针对于IP
uint16_t ntohs(uint16_t netshort);  //net to host  针对于PORT
```

##### ip地址转换函数

```c
//点分十进制转网络字节序  // 支持ipv4 和 ipv6
#include <arpa/inet.h>
int inet_pton(int af, const char *src, void *dst);
// 参数： 
//   af：AF_INET AF_INET6  选择转换格式
//   src：传入参数 ip地址 点分十进制字符串
//   dst：传出参数 转换后的网络字节序
// 返回值：
//   成功：1
//   异常：0  src指向的不使一个有效的IP地址
//   失败：-1 errno

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
// 参数：
//   af：AF_INET AF_INET6  选择转换格式
//   src：网络字节序的ip地址
//   dst：转换后的本地字节序
//   size：传出参数的大小

// 返回值：
//   成功：电视台
//   失败：NULL 
```

##### sockaddr数据结构

sockaddr地址结构
![sockaddr地址结构](Linux_network_program.assets/sockaddr.png)

```c
struct sockaddr_in {
  sa_family_t    sin_family; /*address family: AF_INET*/
  in_port_t      sin_port;   /*port in network byte order*/
  struct in_addr sin_addr;   /*internet address*/
};

/* Internet address. */
struct in_addr {
  uint32_t       s_addr;     /* address in network byte order */
};

// 赋值
sockaddr_in addr;
addr.sin_family = AF_INET / AF_INET6;
addr.sin_port = htons(9527);

int dst;
inet_pton(AF_INET, "192.168.6.1", (void*)&dst);
addr.sinaddr.s_addr = dst;

addr.sinaddr.s_addr = inet_addr("192.168.6.1");

addr.sin_addr.s_addr = htonl(INADDR_ANY);    //取出系统中有效的任意IP地址

//传参
bind(fd, (struct sockaddr*)&addr, size);


```

##### socket模型创建流程

![socket创建流程](Linux_network_program.assets/socket_create.png)

| 客户端                   | 服务端                         |
| ------------------------ | ------------------------------ |
| socket()                 | socket()                       |
| connect() //建立连接 绑定ip和端口     | bind(); //绑定IP port          |
| write()                  | listen()  //设置监听上限       |
| read()                   | accept()  //阻塞监听客户端链接 |
| close()                  | read()                         |
|                          | write()                        |
|                          | read()                         |
|                          | close()                        |
|                          |                                |

###### 函数介绍

```c
#include <sys/socket.h>
int socket(int domain, int type, int protocol);  // 创建套接字
// 参数：
//   domain  指定ip地址协议  AF_INET、AF_INET6、AF_UNIX
//   type   指定数据传输协议  SOCK_STREAM（TCP）   SOCK_DGRAM（UDP）
//   protocol   一般传0
// 返回值：
//   成功：新套接字对应的文件描述符
//   失败：-1 errno

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); // 给socket绑定一个地址结构（ip port）
参数：
  socketfd 套接字的文件描述符
  addr 传入参数 地址结构
  addrlen 地址结构大小 sizeof()

返回值：
  成功 0
  失败 -1 errno 

int listen(int sockfd, int backlog);  // 设置同时与服务器建立链接的上限数
// 参数：
//   sockfd socket文件描述符
//   backlog 上限值 最大128
// 返回值：
//   成功 0
//   失败 -1 errno

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);  // 阻塞等待客户端建立连接 成功返回一个与客户端成功连接的socket文件描述符
// 参数：
//   socket 用于监听的socket文件描述符
//   addr  传出参数 客户端的地址结构（ip+port）
//   addrlen  传入传出参数 传入addr大小 传出客户端addr的实际大小  sizeof 

// 返回值：
//   成功 进行通信的文件描述符
//   失败 -1 errno


int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);  //连接服务端
参数：
  socket :socket函数返回值
  addr： 服务器的地址结构
  addrlen: 服务器地址结构大小
返回值：


```

###### tcp客户端

```c
int main(int argc,char *argv[])
{
  int ret;
  int cli_fd;
  struct sockaddr_in ser_addr;

  cli_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(cli_fd == -1){
    sys_err("socket err");
  }
  ser_addr.sin_family = AF_INET;
  ser_addr.sin_port = htons(12500);
  ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  ret = connect(cli_fd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
  if(ret == -1){
    sys_err("conect err");
  }
  while(1){
    char buf[1024];
    
    scanf("%s", buf);
    write(cli_fd, buf, sizeof(buf));
  }
  close(cli_fd);
  return 0;
}
```

###### tcp服务端

```c

int main(int argc,char *argv[])
{
  int ret;
  char buf[1024];
  int lfd,ser_fd;
  struct sockaddr_in ser_addr, c_addr;

  lfd = socket(AF_INET, SOCK_STREAM, 0);
  if(lfd == -1){
    sys_err("socket err");
  }

  ser_addr.sin_family = AF_INET;
  ser_addr.sin_port = htons(12500);
  ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  ret = bind(lfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
  if(ret == -1){
    sys_err("bind err");
  }

  ret = listen(lfd, 255); 
  if(ret == -1){
    sys_err("listen err");
  }

  socklen_t cli_addr_len = sizeof(c_addr);
  ser_fd = accept(lfd, (struct sockaddr*)&c_addr, &cli_addr_len);
  if(ser_fd == -1){
    sys_err("accept err");
  }

  printf("client connect success: ip = %s port = %d\n",inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));

  while(1){
    
    ret = read(ser_fd, buf, sizeof(buf));
    buf[ret] = '\0';   // 使字符串末位赋0 防止乱码
    if(ret == 0){
      break;
    }
    printf("from clint: %s\n", buf);  //使用换行符，使服务端能刷新缓冲区
  }
  close(lfd);
  close(ser_fd);
  return 0;
}
```
