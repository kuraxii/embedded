#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    int i;
    pid_t pid, wpid;

    for (i = 0; i < 5; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            break;
        }
    }

    if (i == 5)
    {
        // while((wpid = waitpid(-1,NULL,0))){  //阻塞方式回收子进程

        //   printf("wait child %d \n",wpid);
        // }

        while (((wpid = waitpid(-1, NULL, WNOHANG))) != -1)
        { //非阻塞方式回收子进程

            if (wpid > 0)
            {
                printf("wait child %d \n", wpid);
            }
            else if (wpid == 0)
            {
                printf("no child die\n");
            }

            sleep(1);
        }
    }
    else
    {
        sleep(i);
        printf("i am child， pid = %d\n", getpid());
    }

    return 0;
}