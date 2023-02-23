#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
void sys_err(char* str){
  perror(str);
  exit(1);
}
int main(int argc,char *argv[])
{
 
  size_t i;
  pid_t pid;
  int pipefd[2];
  int ret = pipe(pipefd);
  for(i = 0; i < 2; i++){
    pid = fork();
    if(pid == -1)
      sys_err("fork err"); 
    if(pid == 0)
      break;
  }
  if(i == 2){
    close(pipefd[0]);
    close(pipefd[1]);
    wait(NULL);
    wait(NULL);
  }
  else if(i == 0){  //兄进程
    close(pipefd[0]);
    
    dup2(pipefd[1],STDOUT_FILENO);
    
    execlp("ls","ls",NULL);

  }else if (i == 1)  //弟进程
  {
    close(pipefd[1]);
    
    dup2(pipefd[0],STDIN_FILENO);
    
    execlp("wc","wc","-l",NULL);
  }
  



  return 0;
}