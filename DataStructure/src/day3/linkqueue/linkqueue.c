#include "linkqueue.h"
#include <malloc.h>


// 创建队列
QUEUE* CreateQueue()
{
    NODE* node = malloc(sizeof(NODE));
    if(node == NULL)
    {
        perror("malloc");
        return NULL;
    }
    node->next = NULL;
    

    QUEUE* head = malloc(sizeof(QUEUE));
    if(head == NULL)
    {
        perror("malloc");
        return NULL;
    }
    head->rear = node;
    head->front = NULL;

    head->len = 0;

    return head;
}

// 入队列
void pushQueue(QUEUE* st, Data data)
{
    if(st == NULL)
    {
        printf("Queue is NULL\n");
        return;
    }

    NODE* node = malloc(sizeof(NODE));
    if(node == NULL)
    {
        perror("malloc");
        return;
    }
    node->data = data;
    NODE* curr = st->rear;
    while(curr->next)
    {
        curr = curr->next;
    }

    curr->next = node;
    node->next = NULL;
    st->front = node;
    st->len++;
}

// 出队列
Data popQueue(QUEUE* st)
{
    if(isEmptyQueue(st))
    {
        printf("the Queue is empty\n");
    }
    NODE* curr = st->rear, *entry;

    entry = curr->next;
    Data data = entry->data;
    curr->next = entry->next;
    free(entry);
    st->len--;

    return data;
}

// 判断空
int isEmptyQueue(QUEUE* st)
{
    if(st->front == st->rear)
    {
        printf("Queue is NULL\n");
    }

    return st->front == NULL ? 1 : 0;
}

// 清空队列
void clearQueue(QUEUE* st)
{

    NODE* curr = st->rear, *entry;

    while(curr->next)
    {
        entry = curr->next;
        curr->next = entry->next;
        free(entry);
    }
    st->len = 0;

}

int getLength(QUEUE *st)
{
    return st->len;
}
