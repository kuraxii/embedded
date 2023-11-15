#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
struct tval
{
    int ret;
    char *str;
};
void sys_err(char *str)
{
    perror(str);
    exit(1);
}

void *func(void *arg)
{
    struct tval *ret;
    ret = (struct tval *)malloc(sizeof(struct tval));
    ret->ret = (intptr_t)arg;
    ret->str = "hellow word";

    return ret;
}

int main(int argc, char *argv[])
{
    pthread_t tid[5];
    int ret, i;
    struct tval *retval;

    for (i = 0; i < 5; i++)
    {
        ret = pthread_create(&tid[i], NULL, func, (void *)(intptr_t)i);
        if (ret != 0)
        {
            sys_err("pthread_create error");
        }
    }

    for (i = 0; i < 5; i++)
    {
        ret = pthread_join(tid[i], (void **)&retval);
        if (ret != 0)
            sys_err("pthread_join error");
        printf("retval->ret = %d,\nretval-> str = %s\n", retval->ret, retval->str);
    }

    pthread_exit(NULL);
}