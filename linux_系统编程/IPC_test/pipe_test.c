#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
void sys_err(char *str){
  perror(str);
  exit(1);
}
int main(int argc,char *argv[])
{

  int ret;
  pid_t pid;
  int pipefd[2];
  ret = pipe(pipefd);
  char *str = "hello pipe\n";
  char buf[1024];

  pid = fork();
  if(pid > 0){   //父进程
    
     
    close(pipefd[1]);
    dup2(pipefd[0],STDIN_FILENO);
    execlp("wc","wc","-l",NULL);
  }else if (pid == 0){ //子进程
    close(pipefd[0]);
    dup2(pipefd[1],STDOUT_FILENO);
    execlp("ls","ls",NULL);
  }else{
    sys_err("fork err");
  }
  return 0;
}