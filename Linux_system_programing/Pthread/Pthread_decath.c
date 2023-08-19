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

void *func(void *arg)
{
    printf("thread pid = %d thread id= %ld\n", getpid(), pthread_self());
}

int main(int argc, char *argv[])
{

    pthread_t tid;
    int ret;
    ret = pthread_create(&tid, NULL, func, NULL);
    if (ret != 0)
    {
        fprintf(stderr, "pthread_create error : %s\n", strerror(ret));
    }

    ret = pthread_detach(tid); //设置线程分离
    if (ret != 0)
    {
        fprintf(stderr, "pthread_create error : %s\n", strerror(ret));
    }

    ret = pthread_join(tid, NULL);
    printf("join ret = %d\n", ret);
    if (ret != 0)
    {
        fprintf(stderr, "pthread_create error : %s\n", strerror(ret));
    }

    pthread_exit(NULL);
}