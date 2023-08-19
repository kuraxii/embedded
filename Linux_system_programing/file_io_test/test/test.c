#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    int fd = open(argv[1], O_RDWR);
    if (fd == -1)
    {
        perror("open file fail!");
        exit(1);
    }
    int len = lseek(fd, 110, SEEK_END);
    printf("lseek get len = %d\n", len);
    printf("%ld\n", write(fd, "a", 1));

    close(fd);

    return 0;
}