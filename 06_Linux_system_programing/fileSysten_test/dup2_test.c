#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    int fd1, fd2;
    fd1 = open(argv[1], O_RDWR);
    fd2 = open(argv[2], O_RDWR);
    printf("fd1 = %d fd2 = %d\n", fd1, fd2);

    int ret;
    ret = dup2(fd1, fd2);
    printf("fd1 = %d fd2 = %d\n", fd1, fd2);
    printf("%d\n", ret);

    dup2(fd1, STDOUT_FILENO);

    close(fd1);

    return 0;
}