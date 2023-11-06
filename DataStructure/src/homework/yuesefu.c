#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

typedef int Data;
typedef struct Node
{
    Data data;
    struct Node *next;
    struct Node *p;
} Node, *Linklist;

Linklist CreateEmptyLink()
{
    Linklist head = (Linklist)malloc(sizeof(Node));
    if (head == NULL)
    {
        perror("malloc:");
    }
    head->data = -1;
    head->next = head;
    return head;
}

int insert(Linklist head, Data data)
{
    if (head == NULL)
    {
        printf("Linklist is NULL\n");
        return -1;
    }
    Linklist curr = head, q;
    while (curr->next != head)
    {
        curr = curr->next;
    }
    q = malloc(sizeof(Node));
    curr->next = q;
    q->data = data;
    q->next = head;
    return 0;
}
int delete(Linklist *head)
{
    Linklist curr = *head;
    while(curr->next != *head)
    {
        curr = curr->next;
    }
    curr->next = (*head)->next;
    free(*head);
    *head = curr->next;
    return 0;
}

void print(Linklist head)
{
    Linklist curr = head->next;
    while(curr->next != head->next){
        printf("%d ", curr->data);
        curr = curr->next;
    }


    printf("\n");


}


int main()
{
    int num, m;
    printf("请输入总人数和m值:\n");
    scanf("%d %d", &num, &m);
    Linklist head =  CreateEmptyLink();
    for (int i = 1; i < num; i++)
    {
        insert(head, i);
    }
    
    int j = m;

    while(head->next != head){
        j = m;
        while(j--){
            head = head->next;
            if(head->data == -1){
                head = head->next;
            }
        }
        printf("%d ", head->data);
        delete(&head);
    }
    printf("\n");

    
    return 0;
}

