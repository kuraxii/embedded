#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
int main(int argc,char *argv[])
{

    int fd1,fd2;
    if((fd1 = open(argv[1],O_RDONLY)) == -1 )
    {  
        printf("读取错误,%s",strerror(errno));
        exit(1);
    }
    if((fd2 = open(argv[2],O_WRONLY | O_CREAT | O_TRUNC ,0777)) == -1 )
    {  
        printf("打开错误,%s",strerror(errno));
        exit(1);
    }
    char buf[1024] = {0};
    int n;
  

    while((n = read(fd1,buf,sizeof(buf))) != 0)
    {
        if(n < 0){
            perror("read error");
            break;
        }
        write(fd2,buf,n);
        
    }
    close(fd1);
    close(fd2);
    
    
    


    return 0;
}

