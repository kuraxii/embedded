#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
void sys_err(char *str)
{
  perror(str);
  exit(1);
}

int main(int argc,char *argv[])
{
  char *p = NULL;
  int ret,fd;

  fd = open("mmap_test", O_RDWR|O_CREAT|O_TRUNC, 0664);
  if(fd == -1){
    sys_err("open error");
  }

  lseek(fd, 10, SEEK_END);
  ret = write(fd, "\0", 1);
  if(ret == -1){
    sys_err("weite err");
  }
  int len = lseek(fd, 0, SEEK_END);

  p = mmap(NULL, len, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
  if(p == MAP_FAILED){
    sys_err("mmap error");
  }

  strcpy(p,"hello mmap");

  printf("%s\n",p);

  return 0;
}