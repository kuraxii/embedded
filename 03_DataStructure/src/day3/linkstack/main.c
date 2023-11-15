#include <stdio.h>
#include "linkstack.h"

int main()
{
    STACK *stack = CreateStack();

    printf("插入10个元素\n");
    for(int i = 0; i < 10; i++)
    {   
        pushStack(stack, i);
    }

    printf("栈长度为%d\n", getLength(stack));

    printf("出栈6个元素:\n");
    for(int i = 0; i < 6; i ++)
    {
        printf("%d ", popStack(stack));
    }
    printf("\n");

    printf("清空栈\n");
    clearStack(stack);
    printf("is empty: %d\n", isEmptyStack(stack));


}
