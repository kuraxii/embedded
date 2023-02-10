#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(int argc,char *argv[])
{
  pid_t pid,wpid;
  int status;
  pid = fork();
  if(pid == 0)
  {
    printf("---child,my id = %d ,going to sleep 10s\n",getpid);
    sleep(10);
  }else if(pid > 0)
  {
    wpid = wait(&status);
    if(wpid == -1)
    {
      perror("wait err");
      exit(1);
    }
    printf("wait success");
  }else
  {
    perror("fork err");
    exit(1);
  }


  return 0;
}