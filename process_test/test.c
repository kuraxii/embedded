#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc,char *argv[])
{
  pid_t pid = fork();
  if(pid ==-1){
    perror("fork error");
    exit(1);
  }else if(pid == 0)
  {
    execlp("a.out","a.out",NULL);
    perror("exec error");
    exit(1);
  }else if (pid > 0)
  {
    printf("I am parent : %d\n",getpid());
  }
 

  return 0;
}