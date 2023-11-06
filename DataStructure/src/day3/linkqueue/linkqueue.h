#ifndef _LINKSTACK_H
#define _LINKSTACK_H

typedef int Data;

typedef struct node{
    Data data;
    struct node* next;
}NODE;

typedef struct queue{
    NODE *rear;
    NODE *front;
    int len;
}QUEUE;




// 创建队列
QUEUE* CreateQueue();

// 入队列
void pushQueue(QUEUE* st, Data data);

// 出队列
Data popQueue(QUEUE* st);

// 判断空
int isEmptyQueue(QUEUE* st);

// 清空栈

void clearQueue(QUEUE* st);

int getLength(QUEUE *st);


#endif
