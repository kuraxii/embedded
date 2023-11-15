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
  int len = lseek(fd, 0, SEEK_END);  //获取文件的大小
  
  p = mmap(NULL, len,PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(p == MAP_FAILED){
    sys_err("mmap error");
  }
  close(fd);
  strcpy(p,"hello mmap");

  printf("%p\n",p);
  p++;
  printf("%p\n",p);

  ret = munmap(p,len);
  if(ret == -1)
  sys_err("munmap err");

  printf("%s\n",p);

  return 0;
}