#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
void sys_err(char *str)
{
  perror(str);
  exit(1);
}

void _waitpid(int sig){
  int ret;
  while((ret = wait(NULL)) != -1){
    
    printf("---catch child pid = %d\n",ret);
  }
  
}

int main(int argc,char *argv[])
{
  int ret, pid;
  char buf[1024];
  int lfd, cfd;
  signal(SIGCHLD, _waitpid);   //使用信号捕捉，及时处理僵尸进程
  lfd = socket(AF_INET, SOCK_STREAM, 0);
  if(lfd == -1){
    sys_err("socket err");
  }
  struct sockaddr_in ser_addr;
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
  struct sockaddr_in c_addr;
  socklen_t c_addr_len = sizeof(c_addr);
  while (1)
  {
    cfd = accept(lfd, (struct sockaddr*)&c_addr, &c_addr_len);
    printf("client connect success: ip = %s port = %d\n",inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
    pid = fork();
    if(pid == 0){   //子进程
      close(lfd);   //fock后立即退出循环，防止后续逻辑混乱
      break;
    }else if (pid > 0)   //父进程
    {
      close(cfd);
      continue;
      
    }else{   //fork err
      sys_err("fork err");
    }
    
  }
  if(pid == 0){
     while(1){
        ret = read(cfd, buf, sizeof(buf));
        buf[ret] = '\0';
        if(ret == 0){
          break;
        }
        printf("from clint: %s", buf);
      }
      close(cfd);
  }
  
  return 0;
}