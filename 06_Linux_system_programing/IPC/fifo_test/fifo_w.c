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
    int ret, i = 0;
    int fd = open(argv[1], O_WRONLY);
    if (fd < 0)
    {
        sys_err("open error");
    }
    char *str = "fifo_w";
    while (1)
    {
        i++;
        printf("%d\n", i);
        sprintf(buf, "%d %s\n", i, str);

        write(fd, buf, strlen(buf));

        sleep(2);
    }
    close(fd);
    return 23;
}