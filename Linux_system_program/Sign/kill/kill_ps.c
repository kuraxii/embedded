#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
void sys_err(char *str)
{
  perror(str);
  exit(1);
}

int main(int argc,char *argv[])
{
  pid_t pid;

  pid = fork();
  if(pid == -1){
    sys_err("fork err");
  }

  if(pid == 0){
    printf("child pid = %d,ppid = %d\n",getpid(),getppid());
    kill(getppid(),SIGKILL);
  }else{
    printf("parent pid = %d\n",getpid());
  }





  return 0;
}