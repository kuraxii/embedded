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
    while (1)
    {
        // printf("i= %ld, thread: pid = %d, tid = %lu\n",(intptr_t)arg, getpid(), pthread_self());
        // sleep(1);
        pthread_testcancel(); //添加取消点
    }
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    int i, ret;

    ret = pthread_create(&tid, NULL, tfn, (void *)(intptr_t)i); //传参采用值传递
    if (ret != 0)
    {
        perror("pthread_create err");
    }

    sleep(5);

    pthread_cancel(tid); //终止线程

    return 0;
}