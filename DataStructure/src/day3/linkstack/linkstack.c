#include "linkstack.h"
#include <malloc.h>


// 创建栈
STACK* CreateStack()
{
    NODE* node = malloc(sizeof(NODE));
    if(node == NULL)
    {
        perror("malloc");
        return NULL;
    }
    node->next = NULL;
    

    STACK* head = malloc(sizeof(STACK));
    if(head == NULL)
    {
        perror("malloc");
        return NULL;
    }
    head->head = node;
    head->top = NULL;

    return head;
}

// 入栈
void pushStack(STACK* st, Data data)
{
    if(st == NULL)
    {
        printf("stack is NULL\n");
        return;
    }

    NODE* node = malloc(sizeof(NODE));
    if(node == NULL)
    {
        perror("malloc");
        return;
    }
    node->data = data;
    NODE* curr = st->head;
    while(curr->next)
    {
        curr = curr->next;
    }

    curr->next = node;
    node->next = NULL;
    st->top = node;
    st->len++;
}

// 出栈
Data popStack(STACK* st)
{
    if(isEmptyStack(st))
    {
        printf("the stack is empty\n");
    }
    NODE* curr = st->head, *entry;
    while(curr->next)
    {
        entry = curr;
        curr = curr->next;
    }
    Data data = curr->data;
    free(curr);
    entry->next = NULL;
    st->top = entry;
    st->len--;

    return data;
}

// 判断空
int isEmptyStack(STACK* st)
{
    if(st->top == st->head)
    {
        printf("stack is NULL\n");
    }

    return st->top == NULL ? 1 : 0;
}

// 清空栈
void clearStack(STACK* st)
{

    NODE* curr = st->head, *entry;

    while(curr->next)
    {
        entry = curr->next;
        curr->next = entry->next;
        free(entry);
    }

    st->len = 0;
    

}


int getLength(STACK *st)
{
    return st->len;
}