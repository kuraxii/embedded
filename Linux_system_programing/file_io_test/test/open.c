#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>



int main(int argc,char *argv[])
{
    int fd;
    // fd = open("./dict.txt",O_RDONLY);   //以只读打开文件
    // fd = open("./dict.cp",O_RDONLY | O_CREAT, 0777);
    fd = open("./dict.cp2",O_WRONLY);
    printf("%d errno = %d :%s\n",fd, errno,strerror(errno));
    
    close(fd);

    return 0; 
}