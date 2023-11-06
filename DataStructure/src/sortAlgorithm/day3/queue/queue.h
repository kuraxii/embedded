#ifndef _QUEUE_H
#define _QUEUE_H
typedef int Data;

typedef struct {    
    Data* queue;
    int front;
    int rear;
    int max;
}QUEUE;

int popQueue(QUEUE *h);
void pushQueue(QUEUE *h, Data data);
QUEUE* CreateQueue(int num);
int isEmptyQueue(QUEUE* h);
int isFullQueue(QUEUE* h);
void clraequeue(QUEUE* h);

int getLength(QUEUE*);
#endif
