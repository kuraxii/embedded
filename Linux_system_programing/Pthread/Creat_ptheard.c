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

void *tfn(void *arg)
{
    sleep((intptr_t)arg);
    printf("i= %ld, thread: pid = %d, tid = %lu\n", (intptr_t)arg, getpid(), pthread_self());
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    int i, ret;
    for (i = 0; i < 5; i++)
    {
        ret = pthread_create(&tid, NULL, tfn, (void *)(intptr_t)i); //传参采用值传递
        if (ret != 0)
        {
            perror("pthread_create err");
        }
    }

    sleep(i);
    printf("main: pid = %d, tid = %lu\n", getpid(), pthread_self());

    return 0;
}