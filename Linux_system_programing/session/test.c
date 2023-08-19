#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
void sys_err(char *str)
{
    perror(str);
    exit(1);
}

int main(int argc, char *argv[])
{

    pid_t pid;
    pid = fork();
    if (pid == -1)
    {
        sys_err("fork err");
    }

    if (pid == 0)
    {
        printf("child prosses PID is %d\n", getpid());
        printf("Grop ID of child   is %d\n", getpgid(0));
        printf("Session ID of child is  %d\n", getsid(0));

        setsid();
        printf("change :\n");
        printf("child prosses PID is %d\n", getpid());
        printf("Grop ID of child   is %d\n", getpgid(0));
        printf("Session ID of child is  %d\n", getsid(0));
    }

    return 0;
}