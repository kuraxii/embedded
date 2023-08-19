#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    char *a = "aaa";
    printf("段错误\n");
    a[0] = 'c';

    return 0;
}