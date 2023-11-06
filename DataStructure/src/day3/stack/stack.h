/*===============================================
*   文件名称：stack.h
*   创 建 者：Null-zzj
*   创建日期：2023年06月25日
*   描    述：
================================================*/
#ifndef _STACK_H
#define _STACK_H
#define MAX 20
typedef int Data;

typedef struct{
    Data *stack;
    int top;
    int base;
}STACK;

STACK* createStack(STACK* h);
Data popstack(STACK *h);
int pushstack(STACK *h, Data data);
int ClearStack(STACK *h);
int isEmptyStack(STACK* h);
int isFullStack(STACK* h);
#endif
