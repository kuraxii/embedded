/*********************************************
* @FileName: test.c
* @Author: Null-zzj
* @Mail: zj.zhu.cn@gmail.com
* @CreatedTime: Mon Jul 03 09:01:18 2023
* @Descript:
*/

#include<apue.h>
#include<error.h>
#include<fcntl.h>
#include <string.h>


char *argv[20] = {0};
int count;


void strucArgv(char *buf)
{
    argv[count++] = strtok(buf, " ");

    while((argv[count] = strtok(NULL," ")) != NULL)
    {
        count++;
    }
    

}

int main()
{
	char str[] = "abc cnczx csdv vsd";


    strucArgv(str);
    printf("count = %d\n", count);
    for(int i = 0; i < count; i++)
    {
        printf("%s\n", argv[i]);
    }


    return 0;
}
