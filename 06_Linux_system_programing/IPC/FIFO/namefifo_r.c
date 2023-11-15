/*********************************************
* @FileName: namefifo.c
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @CreatedTime: Tue Jul 04 09:49:10 2023
* @Descript:
*/

#include<apue.h>
#include<error.h>
#include<fcntl.h>


int main(int argc, char* argv[])
{
    int fd;
    
    if((fd = open("myfifo", O_RDONLY)) == -1)
        err_sys("open");

    int ret;
    char buf[1024];
    while(ret = read(fd, buf, sizeof(buf)))
    {
        if(ret < 0)
            err_sys("read");
            
        write(STDOUT_FILENO, buf, ret);
            
        

    }

    

    return 0;
}
