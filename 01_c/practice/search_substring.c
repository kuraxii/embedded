/*
 * @Filename: search_substring.c
 * @Author: Null-zzj
 * @Date: 23-06-13 18:19
 * @Descript:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>


int main(int argc,char *argv[])
{
    if(argc != 3)
    {
        printf("the argument input err\n");
    }

    FILE* fp;
    if(!(fp = fopen(argv[2], "r")))
    {
        printf("the file %s open error", argv[2]);
    }

    char buf[1024] = {0};
    char *p = NULL;
    while(!feof(fp)){
        fgets(buf, sizeof(buf), fp);
        if(strstr(buf, argv[1]))
        {
            printf("%s",buf);
        }
    }

    return 0;
}