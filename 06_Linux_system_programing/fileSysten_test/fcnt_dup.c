#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    int fd1 = open(argv[1], O_RDWR);
    printf("fd1 = %d\n", fd1);
    int newfd = fcntl(fd1, F_DUPFD,
                      0); // 0被占用，fcntl使用文件描述符表中可用的最小的文件描述符返回
    printf("newfd = %d\n", newfd);
    int newfd2 = fcntl(fd1, F_DUPFD, 7); //使用未被占用>=7的文件描述符
    printf("newfd2 = %d\n", newfd2);

    return 0;
}