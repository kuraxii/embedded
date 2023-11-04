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

void* func(void* cfd){
  int ret;
  char buf[1024];
  
  while(1){
    memset(buf, '\0', sizeof(buf));
    ret = read((int)(intptr_t)cfd, buf, sizeof(buf));
    if(ret == 0){
      break;
    }
    buf[ret] = '\0';
    printf("from client: %s\n",buf);
  }

  return NULL;
}

int main(int argc,char *argv[])
{
  int ret;
  int lfd, cfd;
  pthread_t tid;
  lfd = socket(AF_INET, SOCK_STREAM, 0);
  if(lfd == -1){
    sys_err("socket err");
  }

  struct sockaddr_in ser_addr;
  ser_addr.sin_family = AF_INET;
  ser_addr.sin_port = htons(12500);
  ser_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

  ret = bind(lfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
  if(ret == -1){
    sys_err("bind err");
  }

  ret = listen(lfd, 255);
  if(ret == -1){
    sys_err("listen err");
  }

  struct sockaddr_in cli_addr;
  socklen_t cli_addr_len = sizeof(cli_addr);
  while(1){
    cfd = accept(lfd, (struct sockaddr*)&cli_addr, &cli_addr_len);
    printf("client connect success,ip= %s, port= %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

    ret = pthread_create(&tid, NULL, func, (void*)(intptr_t)cfd);

    pthread_detach(tid);   //设置线程分离

  }
  return 0;
}
