#include <malloc.h>
#include "stack.h"




// 创建空白栈
STACK* createStack(STACK* h)
{
    h = malloc(sizeof(STACK));
    if(h == NULL)
        perror("malloc");

    h->stack = malloc(sizeof(Data) * MAX);
    h->top = -1;
    h->base = 0;
    return h;
}

// 出栈
Data popstack(STACK *h)
{
    if(h == NULL)
    {
        printf("stack is NULL");
        return -1;
    }
    if(h->top < h->base)
    {
        printf("stack is empty");
    }

    int i = h->stack[h->top];
    h->top--;
    return i;
}


// 入栈
int pushstack(STACK *h, Data data)
{
    
    if(isFullStack(h))
    {
        printf("stack id full");
        return -1;
    }

    h->stack[++(h->top)] = data;
    return 0;
}



// 清空
int ClearStack(STACK *h)
{
   
    if(isEmptyStack(h))
    {
        printf("stack is already empty");
        return -1;
    }

    h->top = -1;
    return 0;

}

// 判空
int isEmptyStack(STACK* h)
{
    if(h == NULL)
    {
        printf("stack is NULL");
        return -1; 
    }
    return h->top < h->base ? 1 : 0;
}

// 判满
int isFullStack(STACK* h)
{
    if(h == NULL)
    {
        printf("stack is NULL");
        return -1; 
    }
    return h->top >= MAX ? 1 : 0;
}























