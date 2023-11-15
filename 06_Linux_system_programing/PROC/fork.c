/*********************************************
* @FileName: fork.c
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @CreatedTime: Mon Jul 03 10:24:43 2023
* @Descript:
*/

#include<apue.h>
#include<error.h>
#include<fcntl.h>
#include<sys/wait.h>


void handler(int sig)
{
    printf("catch the sig %d\n", sig);
}

int main(int argc, char* argv[])
{
    signal(SIGCHLD, handler);


    pid_t pid;
    printf("hello ");
    if((pid = fork()) == 0)
    {
        printf("i am a child\n");
        
        
    }else{
        printf("i am a parent\n");
        wait(NULL);
    }

    printf("i am %d, i am done\n", pid);

    return 0;

  
}