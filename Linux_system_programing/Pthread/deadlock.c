#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
void sys_err(char *str, int _errno)
{
    fprintf(stderr, "%s :%s\n", str, strerror(_errno));
}

void *func1(void *arg); //两个线程体
void *func2(void *arg);

int count = 0;
pthread_mutex_t mutex1, mutex2;

int main(int argc, char *argv[])
{
    int i, ret;
    pthread_t tid[2];
    pthread_mutex_init(&mutex1, NULL);

    ret = pthread_create(&tid[0], NULL, func1, NULL);
    if (ret != 0)
    {
        sys_err("pthread_create error", ret);
    }

    ret = pthread_create(&tid[1], NULL, func2, NULL);
    if (ret != 0)
    {
        sys_err("pthread_create error", ret);
    }

    for (i = 0; i < 2; i++)
    {
        ret = pthread_join(tid[i], NULL);
        if (ret != 0)
        {
            sys_err("pthread_join error", ret);
        }
    }

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_exit(NULL);
}

void *func1(void *arg)
{
    pthread_mutex_lock(&mutex1);
    sleep(1);
    pthread_mutex_lock(&mutex2);

    count++;
    printf("func1 count = %d,  pid = %d, tid = %ld\n", count, getpid(), pthread_self());

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
}

void *func2(void *arg)
{
    pthread_mutex_lock(&mutex2);
    sleep(1);
    pthread_mutex_lock(&mutex1);
    count++;
    printf("func2  count = %d,  pid = %d, tid = %ld\n", count, getpid(), pthread_self());

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    return NULL;
}