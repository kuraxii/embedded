#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
void sys_err(char *str)
{
    perror(str);
    exit(1);
}

int main(int argc, char *argv[])
{
    char buf[1024];
    int ret;
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        sys_err("open error");
    }
    while (1)
    {
        ret = read(fd, buf, sizeof(buf));

        write(STDOUT_FILENO, buf, ret);
        sleep(1);
    }

    return 0;
}