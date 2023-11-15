/*===============================================
*   文件名称：main.c
*   创 建 者：Null-zzj
*   创建日期：2023年06月25日
*   描    述：
================================================*/
#include <stdio.h>
#include <unistd.h>
#include "stack.h"
#include <time.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{ 
    STACK *head = createStack(head);
    srand((unsigned)time(NULL));

    for(int i = 0; i < 10; i ++)
        pushstack(head, rand()%50 + 1);

    for(int i = 0; i < 10; i++)
        printf("%d\n", popstack(head));
    return 0;
} 
