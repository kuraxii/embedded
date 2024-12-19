#include "queue.h"
#include <malloc.h>

QUEUE* CreateQueue(int num)
{
    QUEUE *h = malloc(sizeof(QUEUE));
    if(h == NULL)
    {
        perror("malloc");
    }

    h->queue = malloc(sizeof(Data) * num);
    if(h->queue == NULL)
    {
        perror("malloc");
    }
    h->front = 0;
    h->rear = 0;
    h->max = num;
    return h;
}


void pushQueue(QUEUE *h, Data data)
{   
    if(isFullQueue(h))
    { 
        printf("queue is full\n");
        return ;
    }
    h->queue[h->rear++] = data;

   
    h->rear %= h->max;
    

}


int popQueue(QUEUE *h)
{
    if(isEmptyQueue(h))
    { 
        printf("queue is empty\n");
        return -1;
    }
    int i = h->queue[h->front];
    h->front++;
    
    h->front %= h->max;
    
    return i;
}

int isFullQueue(QUEUE* h)
{
    if(h == NULL)
    {
        printf("queue == NULL\n");
        return -1;
    }
    return (h->rear + 1) % h->max == h->front ? 1 : 0;   // 循环队列无法放满
}

int isEmptyQueue(QUEUE* h)
{
    if(h == NULL)
    {
        printf("queue == NULL\n");
        return -1;
    }

    return h->front == h->rear ? 1 : 0;

}


void clearqueue(QUEUE* h)
{
    h->rear = h->front;

}


int getLength(QUEUE* q)
{
    if(q == NULL)
    {
    printf("queue is NULL");
    }

    return ((q->rear - q->front) + q->max) % q->max;

}



















