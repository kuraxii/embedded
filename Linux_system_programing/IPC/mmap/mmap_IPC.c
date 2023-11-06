#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
void sys_err(char *str)
{
    perror(str);
    exit(1);
}

int main(int argc, char *argv[])
{
    pid_t pid;
    char *p;
    int ret, fd;

    if (argc < 2)
    {
        sys_err("too few file input");
    }

    fd = open(argv[1], O_RDWR | O_CREAT, 0664);
    if (fd == -1)
    {
        sys_err("open error");
    }

    int len = lseek(fd, 0, SEEK_END);

    if (len == 0)
    {
        lseek(fd, 100, SEEK_END);
        write(fd, "\0", 1);
    }
    len = lseek(fd, 0, SEEK_END);

    p = (char *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED)
    {
        sys_err("mmap error");
    }

    printf("i am read process  i read mmap: %s\n", p);

    return 0;
}