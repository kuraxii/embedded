#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
int main(int argc,char *argv[])
{

  printf("before fork-1-\n");
  printf("before fork-2-\n");
  printf("before fork-3-\n");
  printf("before fork-4-\n");
  pid_t pid = fork();
  if(pid ==-1){
    perror("fork error");
    exit(1);
  }else if(pid == 0){
    printf("---child is create\n");
    printf("i am child ,my pid id %d\nmy parent pid id %d\n",getpid(),getppid());
  }else if (pid > 0)
  {
    printf("---parent press: my child is %d\n",pid);
    printf("i am parent, my pid is %d\n",getpid());
  }
  printf("end of file\n");

  return 0;
}