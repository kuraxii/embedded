#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
void sys_err(char *str)
{
  perror(str);
  exit(1);
}
 
int main(int argc,char *argv[])
{
 int lfd, connfd, clientfd[FD_SETSIZE];    //自定义数组client，防止遍历1024个文件描述符
 int ret, maxfd, nread, maxi, i;
  char buf[BUFSIZ], str[INET_ADDRSTRLEN];

  lfd = socket(AF_INET, SOCK_STREAM, 0);
  if(lfd == -1){
    sys_err("socket err");
  }
  int opt = 1;
  ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  struct sockaddr_in ser_addr;
  ser_addr.sin_family = AF_INET;
  ser_addr.sin_port = htons(12500);
  ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  ret = bind(lfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr)); 

  ret = listen(lfd, 255);
  maxi = -1;
  for(int i = 0; i < FD_SETSIZE; i++){
    clientfd[i] = -1;
  }

  fd_set r_set, all_set;   /* rset 读事件文件描述符集合  allset用来暂存*/
  maxfd = lfd;
  FD_ZERO(&all_set);
  FD_SET(lfd, &all_set);   /*添加lfd到监听集合*/
  
  while(1){
    r_set = all_set;

    // printf("before select\n");
    // for(int i = lfd + 1; i <= maxfd; i++){
    //   printf("%d -> %d\n",i, FD_ISSET(i, &r_set));
    // }

    nread = select(maxfd + 1, &r_set, NULL, NULL, NULL);
    // printf("after select\n");
    if(nread < 0){    //判断select监听是否出错
        sys_err("select err");
      } 
    if(nread > 0){     //没有新的监听事件，则循环监听
      
      if(FD_ISSET(lfd, &r_set)){    //判断listen套接字满足监听条件，就建立新的连接
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        connfd = accept(lfd, (struct sockaddr*)&client_addr, &client_addr_len);
        if(connfd == -1){ 
          sys_err("accept err");
        }
        printf("client connect success: ip = %s, port = %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        FD_SET(connfd, &all_set);
        
        for(i = 0; i < FD_SETSIZE; i++){  // 添加客户端数组
          if(clientfd[i] == -1){
            clientfd[i] = connfd;
            break;
          }
        }
        if(i > maxi){   
          maxi = i;
        }

    
        if(connfd > maxfd){
          // printf("clientfd > maxfd---\n");
          maxfd = connfd;   //将产生的fd添加到监听集合中，监听数据的读事件
        }
        if(--nread == 0){      //只有lfd有新事件，后续读套接字不执行
        // printf("1\n");
          continue;          
        }
      }

      for(int i = 0; i <= maxi; i++){
        // printf("%d\n",i);
        if(FD_ISSET(clientfd[i], &r_set)){

          ret = read(clientfd[i], buf, sizeof(buf));
          if(ret == -1){
            sys_err("read err");
          }
          if(ret == 0){
            FD_CLR(clientfd[i], &all_set);
            close(clientfd[i]);
          }
          if(ret > 0){
            buf[ret] = '\0';
            printf("%s\n", buf);
          }
          
        }
      }
    }
  }

  return 0;
}