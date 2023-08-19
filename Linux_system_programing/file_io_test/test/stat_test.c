#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    struct stat sbuf;

    int ret = stat(argv[1], &sbuf);
    if (ret == -1)
    {
        perror("stat error");
        exit(0);
    }

    if (S_ISREG(sbuf.st_mode))
    {
        printf("it is regular file\n");
    }
    else if (S_ISDIR(sbuf.st_mode))
    {
        printf("it is a directory\n");
    }
    else if (S_ISCHR(sbuf.st_mode))
    {
        printf("it is character\n");
    }
}