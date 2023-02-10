#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
int main(int argc,char *argv[])
{
  int fd = open("ps.out",O_RDWR | O_CREAT,0664);

  pid_t pid = fork();
  if(pid == 0){
    printf("i am parent");
  }else if(pid > 0){
    dup2(fd,STDOUT_FILENO);
    execlp("ps","ps","aux",NULL);
    
  }



  return 0;
}