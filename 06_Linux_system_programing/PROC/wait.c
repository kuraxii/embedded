/*********************************************
* @FileName: wait.c
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @CreatedTime: Mon Jul 03 15:06:39 2023
* @Descript:
*/

#include<apue.h>
#include<error.h>
#include<fcntl.h>
#include<sys/wait.h>

void pr_exit(int status);

int main(int argc, char* argv[])
{
    pid_t pid;
    int status;

    if((pid = fork()) < 0)
    {
        perror("fork");
        exit(-1);
    }else if(pid == 0)
    {
        sleep(1);
        printf("in child\n");
        exit(134);
    }

    if(wait(&status) != pid)
    {
        perror("wait");
        exit(-1);
    }
    printf("in parent");
    pr_exit(status);
    
    if((pid = fork()) < 0)
    {
        perror("fork");
        exit(-1);
    }
    else if(pid == 0)
    {
        pause();
        // abort();
    }

    if(wait(&status) != pid)
    {
        perror("wait");
        exit(-2);
    }
    pr_exit(status);

    if((pid = fork()) < 0)
    {
        perror("fork");
        exit(-1);
    }else if(pid == 0)
    {
        status /= 0;
    }
    if(wait(&status) != pid)
    {
        perror("wait");
        exit(-1);
    }
    pr_exit(status);

    exit(0);
    return 0;
}


void pr_exit(int status)
{
    if(WIFEXITED(status))
        printf("normal termination, low-ordor 8 bit of exit status  = %d\n", WEXITSTATUS(status));
    else if(WIFSIGNALED(status))
    {
        printf("abnormally termination, signal number = %d\n", WTERMSIG(status));
    }

}









