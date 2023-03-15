#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


void sys_err(char *str)
{
  perror(str);
  exit(1);
}

int main(int argc, char* argv[])
{
  pid_t pid,ret;
  int fd;
  if((pid = fork()) == -1){
    sys_err("fork err");
  }

  if(pid == 0){
  setsid();

  chdir("/");
  
  umask(0422);
  close(STDIN_FILENO);
  fd = open("/dev/null",O_RDWR);
  dup2(fd,STDOUT_FILENO);
  dup2(fd,STDERR_FILENO);
  while(1);
  }




}	
