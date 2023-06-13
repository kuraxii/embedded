/*
 * @Filename: check_char.c
 * @Author: Null-zzj
 * @Date: 23-06-12 23:39
 * @Descript: 编写一个程序，以一个字符和任意文件名作为命令行参数。
 * 如果宇符后面没有参数，该程序读取标准输入；否则，程序依次打开每个文件并报告每个文件中该字符出现的次数。
 * 文件名和字符本身也要一同报告。程序应包含错误检查，以确定参数数量是否正确和是否能打开文件。
 * 如果无法打开文件，程序应报告这一情况，然后继续处理下一个文件。

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

void check_input(char ch)
{
    int num = 0;
    printf("please input a string:\n");
    char str[1024] = {0};
    scanf("%s", str);
    char *p = str;
    
    for(; *p; p++)
    {
        
        if(ch == *p)
        {
            num ++;
        }
    }
    printf("this string have %d char %c\n", num, ch);
}

void check_file(int argc, char* argv[])
{
    int num = 0;
    FILE* p = NULL;
    for (int i = 2; i < argc; i++)
    {
        num = 0;
        if(!(p = fopen(argv[i], "r")))
        {
            printf("the file %s open fail\n", argv[i]);
            continue;
        }
        while(!feof(p)){
            char ch = getc(p);

            if(ch == *argv[1]){
                num ++;
            }
        }
        printf("the file %s has %d char %c\n", argv[i], num, *argv[1]);
    }
    
}


int main(int argc,char *argv[])
{
    if(argc < 2)
    {
        printf("%s: cannot check: no char input\n", argv[0]);
    }    
    if(argc == 2)
    {
        check_input(*argv[1]);
    }
    if(argc >= 2)
    {
        check_file(argc, argv);
    }
    return 0;
}