#include <stdio.h>
#include "linkqueue.h"

int main()
{
    QUEUE *queue = CreateQueue();

    printf("插入10个元素\n");
    for(int i = 0; i < 10; i++)
    {   
        pushQueue(queue, i);
    }
    printf("队列长度为%d\n", getLength(queue));
    printf("出栈6个元素:\n");
    for(int i = 0; i < 6; i ++)
    {
        printf("%d ", popQueue(queue));
    }
    printf("\n");


    printf("清空栈\n");
    clearQueue(queue);
    printf("is empty: %d\n", isEmptyQueue(queue));


}
