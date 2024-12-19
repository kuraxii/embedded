#ifndef _LINKLIST_H
#define _LINKLIST_H

typedef int Data;
typedef struct Node{
    Data data;
    struct Node* next;
}Node, *Linklist;

Linklist CreateEmptyLink();   // 创建空链表
int isEmptyLinklist(Linklist head);  // 判断是否为空
void VisitLinklist(Linklist head);   // 遍历链表
int LengthLinklist(Linklist head);   // 返回链表长度
int SearchLinklist(Linklist head, Data data); //查找数据
int GetLinklist(Linklist head, int pos, Data* data);  // 读取指定位置的数据

int InsertLinklistByPos(Linklist *head, Data data, int pos);
int InsertLinklisByEnd(Linklist head, Data data);  // 尾插
int InsertLinklisByHead(Linklist *head, Data); // 头插
int InsertLinklistByInc(Linklist *head, Data data); // 按升序插入

int DeleteLinklistByPos(Linklist *head, int pos);   // 删除指定位置的节点
void DeleteLinklistByElement(Linklist *head, Data data); // 删除指定的元素
void ClearLinklist(Linklist *head); // 清空链表

void ReverseLinklist(Linklist head); // 单链表反转

int AlterLinklistByPos(Linklist head, Data data, int pos); // 按位置修改


int AlterLinklistByElement(Linklist head, Data src, Data dest); // 按元素修改


#endif
