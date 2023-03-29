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

  ret = bind(lfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
  if(ret == -1){
    sys_err("bind err");
  }

  ret = listen(lfd, 255);

  int epfd = epoll_create(1024);
  if(epfd == -1){
    sys_err("epoll_create err");
  }
  struct epoll_event tep,ep[1024];

  ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &tep);
  if(ret == -1){
    sys_err("epoll_ctl err");
  }

  while(1){
    epoll_wait(epfd, ep, 1024, -1);
    for(int i = 0; i < 1024; i++){
      if(ep[i].events & EPOLLIN){
        if(ep[i].data.fd == lfd){   // new client  connection request 
          struct sockaddr_in cli_addr; 
          socklen_t cli_len = sizeof(cli_addr);
          int cfd = accept(lfd, (struct sockaddr*)&cli_addr, &cli_len);
          if(cfd == -1){
            sys_err("accept err");
          }
          printf("client ip: %s, port: %d", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
          tep.events = EPOLLIN;
          tep.data.fd = cfd;
          ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &tep);
          if(ret == -1){
            sys_err("epoll_ctl err");
          }
        }else{        // client send data
          int len = read(ep[i].data.fd, buf, sizeof(buf));
          if(len == -1){
            sys_err("read err");
          }else if(len == 0){ // client close
            ret = epoll_ctl(epfd, EPOLL_CTL_DEL, ep[i].data.fd, NULL);
            if(ret == -1){
              sys_err("epoll_ctl err");
            }
            close(ep[i].data.fd);
            printf("client close");
          }else{ // read data
            write(STDOUT_FILENO, buf, len);
          }

        }

      }

    }



  }



  return 0;
}