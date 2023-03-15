#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
void sys_err(char *str){
  perror(str);
  exit(1);
}
int main(int argc,char *argv[])
{

  int ret;
  pid_t pid;
  int pipefd[2];
  ret = pipe(pipefd);
  char *str = "hello pipe\n";
  char buf[1024];

  pid = fork();
  if(pid > 0){   //父进程
    close(pipefd[0]);
    
    write(pipefd[1],str,strlen(str));
    
    close(pipefd[1]);
  }else if (pid == 0){ //子进程
    sleep(1);
    close(pipefd[1]);
    ret = read(pipefd[0],buf,sizeof(buf));   //写要控制写入大小，否则多余内容输出乱码
    printf("%d\n",ret);
    write(STDOUT_FILENO,buf,ret);
    close(pipefd[0]);
   
  }else{
    sys_err("fork err");
  }
  return 0;
}