#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
void sys_err(char *str)
{
  perror(str);
  exit(1);
}

int main(int argc,char *argv[])
{ 
  int lfd, ret;
  char buf[1024];
  lfd = socket(AF_INET, SOCK_STREAM, 0);
  if(lfd == -1){
    sys_err("socket err");
  }

  struct sockaddr_in ser_addr;
  ser_addr.sin_family = AF_INET;
  ser_addr.sin_port = htons(12500);
  ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int opt = 1;
  setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));  // reuse address

  ret = bind(lfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
  if(ret == -1){
    sys_err("bind err");
  }

  ret = listen(lfd, 255);
  if(ret == -1){
    sys_err("listen err");
  }

  int epfd = epoll_create(1024);
  if(epfd == -1){
    sys_err("epoll_create err");
  }

  struct epoll_event tep,ep_arr[1024];
  memset(ep_arr, 0, sizeof(ep_arr));
  tep.events = EPOLLIN | EPOLLET; // EPOLLET: edge trigger
  tep.data.fd = lfd; 





  return 0;
}