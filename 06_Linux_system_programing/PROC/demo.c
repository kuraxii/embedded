/*********************************************
* @FileName: demo.c
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @CreatedTime: Mon Jul 03 20:09:30 2023
* @Descript:
*/

#include<apue.h>
#include<error.h>
#include<fcntl.h>


int main(int argc, char* argv[])
{
    
    pid_t pid;

    if((pid = fork()) < 0)
        err_sys("fork");
    else if(pid == 0){
        pid = setsid();

        chdir("/");
        umask(0);
		for(int i = 0; i < __FD_SETSIZE; i++)
            close(i);
        while(1);
    }else{
        printf("%d\n", pid);
        exit(0);
    }


    return 0;
}
