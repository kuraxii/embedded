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