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