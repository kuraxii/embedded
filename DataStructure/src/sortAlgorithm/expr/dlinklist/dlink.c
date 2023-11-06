#include <malloc.h>
#include "dlink.h"

DNLINK CreadeEmptyDNLink()
{
    DNLINK head = malloc(sizeof(DNODE));
    if(head == NULL)
    {
        perror("malloc");
    }

    head->next = head;
    head->pri = head;
    return head;
}

int isEmptyDLink(DNLINK head)
{
    if(head == NULL)
    {
        printf("DNLink is NULL\n");
        return -1;
    }
    return head->next == NULL ? 1 : 0;

}

void printDNLink(DNLINK head)
{
    if(isEmptyDLink(head))
    {
        printf("DNlink is empty");
    }
    DNLINK curr = head->next;
    while(curr != head)
    {
        printf("%d ", curr->data);
        curr = curr->next;
    }
    printf("\n");

}

int InsertDNLinkByHead(DNLINK head, DATA data)
{
    if(head == NULL)
    {
        printf("DNLink is NULL");
    }

    DNLINK curr = head;
    DNLINK p = malloc(sizeof(DNODE));



    p->data = data;

    

    p->next = curr->next;
    curr->next = p;

    p->pri = curr;
    p->next->pri = p;
    return 0;
}