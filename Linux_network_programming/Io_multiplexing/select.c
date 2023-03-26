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
 int lfd, cfd;
 int ret, maxfd, nread;
  char buf[1024];

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

  fd_set r_set, all_set;   /* rset 读事件文件描述符集合allset用来暂存*/
  maxfd = lfd;
  FD_ZERO(&all_set);
  FD_SET(lfd, &all_set);   /*构造select监控文件描述符集*/
  
  while(1){
    r_set = all_set;

    printf("before select\n");
    for(int i = lfd + 1; i <= maxfd; i++){
      printf("%d -> %d\n",i, FD_ISSET(i, &r_set));
    }

    nread = select(maxfd + 1, &r_set, NULL, NULL, NULL);
    printf("after select\n");
    if(nread < 0){    //判断select监听是否出错
        sys_err("select err");
      } 
    if(nread > 0){     //没有新的监听事件，则循环监听
      
      if(FD_ISSET(lfd, &r_set)){    //判断listen套接字是否满足监听条件
        struct sockaddr_in client_fd;
        socklen_t client_fd_len = sizeof(client_fd);
        cfd = accept(lfd, (struct sockaddr*)&client_fd, &client_fd_len);
        if(cfd == -1){
          sys_err("accept err");
        }
        printf("client connect success: ip = %s, port = %d\n", inet_ntoa(client_fd.sin_addr), ntohs(client_fd.sin_port));
        FD_SET(cfd, &all_set);
        if(cfd > maxfd){
          printf("cfd > maxfd---\n");
          maxfd = cfd;
        }
        if(--nread == 0){      //只有lfd有新事件，后续读套接字不执行
        printf("1\n");
          continue;          
        }
      }

      for(int i = lfd + 1; i <= maxfd; i++){
        printf("%d\n",i);
        if(FD_ISSET(i, &r_set)){

          ret = read(i, buf, sizeof(buf));
          if(ret == -1){
            sys_err("read err");
          }
          if(ret == 0){
            FD_CLR(i, &all_set);
            close(i);
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