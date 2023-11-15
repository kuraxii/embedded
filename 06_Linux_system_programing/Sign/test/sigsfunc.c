#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
void sys_err(char *str)
{
    perror(str);
    exit(1);
}
void printset(sigset_t *set)
{

    for (size_t i = 1; i < 32; i++)
    {
        if (sigismember(set, i))
            putchar('1');
        else
        {
            putchar('0');
        }
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    int ret;
    sigset_t set, oldset, pedset;
    ret = sigemptyset(&set); //设置为0
    if (ret == 1)
    {
        sys_err("sigemptyset err");
    }
    ret = sigaddset(&set, SIGINT); //添加set值
    if (ret == 1)
    {
        sys_err("sigaddset err");
    }
    ret = sigprocmask(SIG_BLOCK, &set, NULL); //添加set到屏蔽字中
    if (ret == 1)
    {
        sys_err("sigprocmask err");
    }

    while (1)
    {
        ret = sigpending(&pedset); //读取当前未决信号集
        if (ret == 1)
        {
            sys_err("sigpending err");
        }
        printset(&pedset); //打印set集
        sleep(1);
    }
    return 0;
}