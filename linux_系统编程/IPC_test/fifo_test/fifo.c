#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>

void sys_err(char *str){
  perror(str);
  exit(1);
}

int main(int argc,char *argv[])
{
  pid_t pid;
  size_t i,ret,fd;
  char buf[1024];
  
  for (size_t i = 0; i < 2; i++)
  {
    if ((pid = fork()) == -1)
    {
      sys_err("fork error");
    }
    if(pid == 0)
      break;
  }
  if(i == 2){
    fd = open(argv[1],O_RDONLY);
    if(ret == -1){
      sys_err("i = 2 open err");
    }
    while (1)
    {
      ret = read(fd,buf,sizeof(buf));
      write(STDOUT_FILENO,buf,ret);
    }
    
    
    for (size_t i = 0; i < 2; i++)
    {
      wait(NULL);
    }
    
  }
  if(i == 0){
    int j = 0;
    char *str = "hello";
    fd = open(argv[1],O_WRONLY);
    while(1){
      j++;
      sprintf(buf,"%d %s",j,str);
      write(fd,buf,sizeof(buf));
      sleep(1);
    }
    
  }else if(i == 1)
  {
    int j = 0;
    char *str = "word";
    fd = open(argv[1],O_WRONLY);
    while(1){
      j++;
      sprintf(buf,"%d %s",j,str);
      write(fd,buf,sizeof(buf));
      sleep(1);
    }

  }
  
  



  return 0;
}
