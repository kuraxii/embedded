#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
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

void *func()
{
    struct tval *ret;
    ret = (struct tval *)malloc(sizeof(struct tval));
    ret->ret = 0;
    ret->str = "hellow word";

    return ret;
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    int ret;
    struct tval *retval;
    ret = pthread_create(&tid, NULL, func, NULL);
    if (ret != 0)
    {
        sys_err("pthread_create error");
    }
    ret = pthread_join(tid, (void **)&retval);
    printf("retval->ret = %d,\nretval-> str = %s\n", retval->ret, retval->str);
    return 0;
}