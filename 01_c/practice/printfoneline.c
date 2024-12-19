/*
 * @Filename: printfoneline.c
 * @Author: Null-zzj
 * @Date: 23-06-13 16:55
 * @Descript: 编写一个程序打开一个文本文件，通过交互方式获得文件名。
 * 通过一个循环，提示用户输入一个文件位置。然后该程序打印从该位置开始到下一个换行符之前的内容。
 * 用户输入负数或非数值字符可以结束输入循环。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>


int main(int argc,char *argv[])
{
    int n;
    char str[1024] = {0};
    FILE* fp;
    if(!(fp = fopen(argv[1], "r")))
    {
        printf("the file %s is not found\n", argv[1]);
        return -1;
    }
    printf("the file %s open success\n",argv[1]);
    
    
    printf("please input a num to locate the file offset:\n");
    while(scanf("%d", &n) == 1 && n >= 0)
    {
        fseek(fp, (long)n, SEEK_SET);
        if(!fgets(str, sizeof(str), fp))
        {
            printf("fget error\n");
        }
        printf("%s\n",str);
        printf("please input a num to locate the file offset:\n");
    }

    return 0;
}