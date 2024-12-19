#include "LinkList.h"
#include <malloc.h>
Linklist CreateEmptyLink()
{
    Linklist head = (Linklist)malloc(sizeof(Node));
    if (head == NULL)
    {
        perror("malloc:");
    }
    head->data = -1;
    head->next = NULL;
    return head;
}

int isEmptyLinklist(Linklist head)
{
    if (head == NULL)
    {
        printf("Linklist is NULL\n");
        return -1;
    }
    return head->next == NULL ? 1 : 0;
}

int LengthLinklist(Linklist head)
{
    if (head == NULL)
    {
        printf("Linklist is NULL\n");
        return -1;
    }
    return head->data;
}

void VisitLinklist(Linklist head)
{
    if (isEmptyLinklist(head))
    {
        printf("Linklist is empty\n");
        return;
    }
    Linklist p = head->next;
    while (p)
    {
        printf("%d ", p->data);
        p = p->next;
    }
    printf("\n");
}

int SearchLinklist(Linklist head, Data data)
{
    if (head == NULL)
    {
        printf("Linklist is NULL\n");
        return -1;
    }

    Linklist p = head->next;
    int i = 1;
    if (p == NULL)
    {
        printf("linklist is NULL");
        return 0;
    }
    while (p)
    {
        if (p->data == data)
        {
            return i;
        }
        p = p->next;
        i++;
    }
    return 0;
}

int GetLinklist(Linklist head, int pos, Data *data)
{
    if (head == NULL)
    {
        printf("Linklist is NULL\n");
        return -1;
    }
    Linklist p = head;
    int j = 1;
    while (p && j < pos)
    {
        p = p->next;
        j++; // 循环结束 j == pos
    }

    if (!p || j > pos)
    {
        printf("第%d个元素不存在", pos);
        return 0;
    }
    *data = p->data;
    return 1;
}

// int InsertLinklistByPos(Linklist head, Data data, int pos) // 插入
// {
//     if (head == NULL)
//     {
//         printf("Linklist is NULL\n");
//         return -1;
//     }
//     Linklist p, q;
//     int j = 0;
//     p = head;

//     while (p && j < pos)
//     {
//         p = p->next;
//         j++;
//     }
//     if (!p || j > pos)
//     {
//         printf("第%d个元素不存在\n", pos);
//         return -1;
//     }

//     q = (Linklist)malloc(sizeof(Node));
//     q->data = data;
//     q->next = p->next;
//     p->next = q;
//     head->data++;
//     return 0;
// }

int InsertLinklistByPos(Linklist *head, Data data, int pos)
{
    if (head == NULL)
    {
        printf("Linklist is NULL\n");
        return -1;
    }
    if(pos < 0 || pos > (*head)->data)
    {
        printf("pos is invial\n");
    }
    Linklist *curr = &(*head)->next;
    Linklist p; 
    while(*curr && pos--)
    {
        curr = &(*curr)->next;
    }

    p = malloc(sizeof(Node));
    p->data = data;
    p->next = (*curr)->next;
    (*curr)->next = p; 
    return 0;
} 

int InsertLinklisByEnd(Linklist head, Data data)
{
    if (head == NULL)
    {
        printf("Linklist is NULL\n");
        return -1;
    }
    Linklist p = head, q;
    while (p->next)
    {
        p = p->next;
    }
    q = malloc(sizeof(Node));
    q->data = data;
    p->next = q;
    q->next = NULL;
    head->data++;
}

// int InsertLinklisByHead(Linklist head, Data data)
// {
//     if (head == NULL)
//     {
//         printf("Linklist is NULL\n");
//         return -1;
//     }
//     Linklist p, q;

//     q = malloc(sizeof(Node));
//     q->data = data;
//     q->next = head->next;
//     head->next = q;
//     head->data++;
//     return 0;
// }

int InsertLinklisByHead(Linklist *head, Data data)
{
    if (*head == NULL)
    {
        printf("Linklist is NULL\n");
        return -1;
    }

    Linklist* curr = &(*head)->next;
    Linklist p;

    p = malloc(sizeof(Node));
    p->data = data;

    p->next = (*curr)->next; 
    (*curr)->next = p;
    (*head)->data++;
    return 0;
}



int InsertLinklistByInc(Linklist *head, Data data)
{
    if (*head == NULL)
    {
        printf("Linklist is NULL\n");
        return -1;
    }
    Linklist *curr;
    curr = &(*head)->next; //    curr->  |head| - | 1 |
    Linklist p;
    while (*curr)
    {
        if ((*curr)->data > data)
        {
            p = malloc(sizeof(Node));
            p->data = data;
            p->next = *curr;
            *curr = p;
            (*head)->data++;
            return 0;
        }
        curr = &(*curr)->next;
    }
    if (*curr == NULL)
    {
        p = malloc(sizeof(Node));
        p->data = data;
        p->next = NULL;
        *curr = p;
        (*head)->data++;

        return 0;
    }
}

int AlterLinklistByPos(Linklist head, Data data, int pos)
{
    if (isEmptyLinklist(head))
    {
        printf("Linklist is empty\n");
        return -1;
    }
    if (pos < 0 || pos > LengthLinklist(head))
    {
        printf("pos is invalid\n");
        return -1;
    }
    Linklist curr = head->next;
    while (pos--)
    {
        curr = curr->next;
    }

    curr->data = data;
    return 0;
}

int AlterLinklistByElement(Linklist head, Data src, Data dest)
{
    if (isEmptyLinklist(head))
    {
        printf("Linklist is empty\n");
        return -1;
    }

    Linklist curr = head->next;
    while (curr)
    {
        if (curr->data == src)
            curr->data = dest;
        curr = curr->next;
    }
    return 0;
}

// int _DeleteLinklistByPos(Linklist head, int pos)
// {
//     if (isEmptyLinklist(head))
//     {
//         printf("Linklist is empty\n");
//         return -1;
//     }

//     int j = 0;
//     Linklist p, q;
//     p = head;
//     while (p && j < pos)
//     {
//         p = p->next;
//         j++;
//     }

//     if (!p || j > pos)
//     {
//         printf("第%d个元素不存在\n", pos);
//         return -1;
//     }

//     q = p->next;
//     p->next = q->next;
//     free(q);
//     head->data--;
//     return 0;
// }

int DeleteLinklistByPos(Linklist *head, int pos)
{
    if (isEmptyLinklist(*head))
    {
        printf("Linklist is empty\n");
        return -1;
    }

    Linklist *curr = &(*head)->next; // curr 指向第0个节点
    Linklist entry;
    while (pos--) // 向后移动 pos 次
    {
        curr = &((*curr)->next);
    }

    entry = *curr;
    *curr = entry->next;
    free(entry);
    (*head)->data--;
    return 0;
}

void DeleteLinklistByElement(Linklist *head, Data data)
{
    if (isEmptyLinklist(*head))
    {
        printf("Linklist is empty\n");
        return;
    }

    Linklist *curr = &(*head)->next;
    Linklist entry;
    while (*curr)
    {
        entry = *curr;
        if (entry->data == data)
        {
            *curr = entry->next;
            free(entry);
            (*head)->data--;
        }
        else
        {
            curr = &entry->next;
        }
    }
}

void ClearLinklist(Linklist *head)
{
    if (isEmptyLinklist(*head))
    {
        printf("Linklist is empty\n");
        return;
    }

    Linklist *curr = &(*head)->next;
    Linklist entry;
    while (*curr)
    {
        entry = *curr;
        *curr = entry->next;
        free(entry);
    }
    (*head)->data = 0;
}

void ReverseLinklist(Linklist head)
{
    if (isEmptyLinklist(head))
    {
        printf("Linklist is empty\n");
        return;
    }

    Linklist curr = head->next;
    Linklist entry;
    Linklist last = head->next;
    while (curr)
    {
        entry = curr; // 头插法
        curr = curr->next;
        entry->next = head->next;
        head->next = entry;
    }
    last->next = NULL;
}