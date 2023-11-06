#ifndef _LINKSTACK_H
#define _LINKSTACK_H

typedef int Data;

typedef struct node{
    Data data;
    struct node* next;
}NODE;

typedef struct stack{
    NODE *head;
    NODE *top;
    int len;
}STACK;




// 创建栈
STACK* CreateStack();

// 入栈
void pushStack(STACK* st, Data data);

// 出栈
Data popStack(STACK* st);

// 判断空
int isEmptyStack(STACK* st);

// 清空栈

void clearStack(STACK* st);

int getLength(STACK *st);


#endif
