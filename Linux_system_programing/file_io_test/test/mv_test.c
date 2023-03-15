#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
int main(int argc,char *argv[])
{
    link(argv[1],argv[2]);
    unlink(argv[1]);
    return 0;
}