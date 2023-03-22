#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
void sys_err(char *str)
{
  perror(str);
  exit(1);
}

int main(int argc,char *argv[])
{
  int lfd, sfd;
  int ret;
  char buf[1024];

  lfd = socket(AF_INET, SOCK_STREAM, 0);
  if(lfd == -1){
    sys_err("socket lfd err");
  }

  struct sockaddr_in ser_addr;
  ser_addr.sin_family = AF_INET;
  ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  ser_addr.sin_port = htons(12500);

  ret = bind(lfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
  if(ret == -1){
    sys_err("bing err");
  }

  ret = listen(lfd, 1);
  if(ret == -1){
    sys_err("listen err");
  } 

  struct sockaddr_in c_addr;
  socklen_t c_addr_len = sizeof(c_addr);
  sfd = accept(lfd, (struct sockaddr*)&c_addr, &c_addr_len);
  if(sfd == -1){
    sys_err("accept err");
  }
  printf("client connect success: ip = %s port = %d\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));

  while(1){
    
    ret = read(sfd, buf, sizeof(buf));
    if(ret == 0){
      break;
    }
    printf("from clint: %s\n", buf);

  }

  close(lfd);
  close(sfd);
   


  return 0;
}