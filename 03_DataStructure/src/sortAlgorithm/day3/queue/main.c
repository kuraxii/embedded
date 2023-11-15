#include <stdio.h>
#include "queue.h"
#include <time.h>
#include <stdlib.h>


int main()
{
    QUEUE* qu = CreateQueue(10);

    srand((unsigned)time(NULL));
 
    for(int i = 0; i < 9; i ++)
        pushQueue(qu, i + 1);
    
    printf("入队元素个数%d\n", getLength(qu));
    printf("出队：\n");
    for(int i = 0; i < 3; i++)
        printf("%d ", popQueue(qu));

    
    printf("\n还剩%d个\n", getLength(qu));




}
