#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
int main(int argc,char *argv[])
{
  printf("---before create\n");
  printf("---before create\n");
  printf("---before create\n");
  size_t count;
  scanf("%ld",&count);
  for (size_t i = 0; i < count; i++)
  {
    pid_t pid = fork();
    if(pid == 0){
      printf("%d i am son process %d created\n",i,getpid());
    }
    else
    {
      printf("%d i am parent process my pid is %d , my son id %d \n",i,getpid(),pid); 
    }
    
  }
  


  return 0;
}