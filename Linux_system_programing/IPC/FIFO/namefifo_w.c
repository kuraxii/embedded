/*********************************************
* @FileName: namefifo_w.c
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @CreatedTime: Tue Jul 04 13:55:43 2023
* @Descript:
*/

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<error.h>
#include<fcntl.h>


void hander(int sig)
{
	printf("catch the sig %d\n", sig);
}

int main(int argc, char* argv[])
{
	signal(SIGPIPE, hander);
    int fd;
    if((fd = open("myfifo", O_WRONLY)) == -1)
    {
        mkfifo("myfifo", 0755);
    }

    if((fd = open("myfifo", O_WRONLY)) == -1)
        err_sys("open");
    int ret;
    char buf[1024];
    while(ret = read(STDIN_FILENO, buf, sizeof(buf)))
    {
        if(ret < 0)
            err_sys("read");
        
        if((ret = write(fd, buf, ret)) < 0)
            err_sys("write");
        

    }
    return 0;
}
