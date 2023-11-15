#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char *argv[])
{

    size_t count;
    size_t i;
    scanf("%ld", &count);
    for (i = 0; i < count; i++)
    {
        if (fork() == 0)
        {
            break;
        }
    }
    if (i == count)
    {
        sleep(count);
        printf("parent process\n");
    }
    else
    {
        sleep(i);
        printf("%ld i am son process my pid %d\n", i + 1, getpid());
    }
    return 0;
}
