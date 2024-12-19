#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <malloc.h>
#include "LinkList.h"

int main(int argc, char *argv[])
{
    Linklist head = malloc(sizeof(Node));
    head->next = malloc(sizeof(Node));
    head->next->data = 3;
    head->next->next = malloc(sizeof(Node));
    head->next->next->data = 4;
    head->data = 1;

    VisitLinklist(head);
    InsertLinklistByPos(&head, 1, 1);
    VisitLinklist(head);
    Data data;

    GetLinklist(head, 1, &data);
    printf("%d\n", data);
    VisitLinklist(head);

    printf("-----\n尾插\n");
    InsertLinklisByEnd(head, 11); // 尾插
    VisitLinklist(head);
    

    printf("-----\n头插\n");
    InsertLinklisByHead(&head, 99); // 尾插
    VisitLinklist(head);

    printf("-----\n按位置插入\n"); 
    InsertLinklistByPos(&head, 5,3);
    VisitLinklist(head);


    printf("-----\n按升序插入\n"); 
    InsertLinklistByInc(&head, 5);
    VisitLinklist(head);

    printf("-----\n按位置修改\n"); 
    AlterLinklistByPos(head, 88, 2);
    VisitLinklist(head);

    printf("-----\n按元素修改\n"); 
    AlterLinklistByElement(head, 4, 66);
    VisitLinklist(head);
    
    

    printf("-----\n删除指定位置的节点\n"); 
    DeleteLinklistByPos(&head, 3);
    VisitLinklist(head);

    printf("-----\n删除指定的元素\n"); 
    DeleteLinklistByElement(&head, 4);
    VisitLinklist(head);

    printf("-----\n单链表反转\n");    
    ReverseLinklist(head); 
    VisitLinklist(head);
    


    printf("-----\n清空链表\n");    
    ClearLinklist(&head);
    VisitLinklist(head);


    


   
    



    





    return 0;
}

