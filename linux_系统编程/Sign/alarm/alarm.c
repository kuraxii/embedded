#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
void sys_err(char *str)
{
  perror(str);
  exit(1);
}

void print(){
  printf("hello\n");
}

int main(int argc,char *argv[])
{ 
  struct itimerval time;

  // signal(SIGALRM,print);

  time.it_value.tv_sec = 1;
  time.it_value.tv_usec = 0;

  time.it_interval.tv_sec = 0;
  time.it_interval.tv_usec = 0;
  
  
  
  setitimer(ITIMER_REAL, &time, NULL);

  for(int i = 0;; i++){
    printf("%d\n",i);
  }



  return 0;
}