#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
void sys_err(char *str)
{
  perror(str);
  exit(1);
}

void catch_child(int signo){
  int ret;
  while((ret = wait(NULL)) != -1){
    
    printf("---catch child pid = %d\n",ret);
  }
}

int main(int argc,char *argv[])
{
    int i,ret;
    pid_t pid;
    signal(SIGCHLD, catch_child);
    for (i = 0; i < 5; i++)
    {
      if((pid = fork()) == 0)
        break;

      if(pid == -1){
        sys_err("fork err");
      }
    }

 
    if(i == 5){   //父进程
      printf("i am parent my pid = %d\n",getpid());

      signal(SIGCHLD, catch_child);
      while(1);
    }else
    {
      printf("i am child my pid = %d\n",getpid());
    }
    
    
  return 0;
}