// 作业:父进程 fork 3个子进程，三个子进程一个调用ps命令，一个调用自定义程序1(正常)
// ，一个调用自定义程序2(会出段错误)。父进程使用waitpid对其子进程进行回收。
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
int main(int argc, char *argv[])
{
    size_t i;
    pid_t wpid;
    for (i = 0; i < 3; i++)
    {
        if (fork() == 0)
        {
            break;
        }
    }
    if (i == 3)
    {
        sleep(3);
        while (1)
        {
            if ((wpid = waitpid(-1, NULL, WNOHANG)) == -1)
            {
                break;
            }
            else
            {
                printf("died son process %d\n", wpid);
            }
        }
    }
    else if (i == 0)
    {
        execlp("ps", "ps", "aux", NULL);
    }
    else if (i == 1)
    {
        execl("./test", "test", NULL);
    }
    else if (i == 2)
    {
        execl("./test1", "test1", NULL);
    }
    else
    {
        perror("fork err");
    }

    printf("\n");

    return 0;
}