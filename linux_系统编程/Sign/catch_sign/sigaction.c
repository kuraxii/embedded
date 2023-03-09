#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
void sys_err(char *str)
{
  perror(str);
  exit(1);
}

void sys_sig(int signo){
  printf("----catch sign %d\n", signo);
  sleep(5);
}

int main(int argc,char *argv[])
{
  struct sigaction act,oldact;
  
  act.sa_handler = sys_sig;
  sigemptyset(&act.sa_mask);
  sigaddset(&act.sa_mask, SIGINT);

  act.sa_flags = 0;
  sigaction(SIGQUIT, &act, &oldact);


  while(1){}

  return 0;
}