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
    pid_t pid, wpid;
    int status;
    pid = fork();
    if (pid == 0)
    {
        printf("---child,my id = %d ,going to sleep 10s\n", getpid());
        sleep(100);
    }
    else if (pid > 0)
    {
        wpid = waitpid(pid, &status, 0);
        if (waitpid == -1)
        {
            perror("waitpid err");
            exit(0);
        }
        printf("i am parent i am kill %ld\n", wpid);
        if (WIFEXITED(status))
        {
            printf("the son prosses exit nor %d\n", WEXITSTATUS(status));
        }
        if (WIFSIGNALED(status))
        {
            printf("the son prosses exit nonor %d\n", WTERMSIG(status));
        }
        if (WIFCONTINUED(status))
        {
            printf("the son prosses stop %d\n", WSTOPSIG(status));
        }
    }
    else
    {
        perror("fork err");
        exit(1);
    }

    return 0;
}