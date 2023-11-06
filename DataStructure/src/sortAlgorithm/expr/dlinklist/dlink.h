#ifndef _DLINK_H
#define _DLINK_H

typedef int DATA;

typedef struct dnode{
    DATA data;
    struct dnode* pri;
    struct dnode* next;
}DNODE, *DNLINK;


DNLINK CreadeEmptyDNLink();

int isEmptyDLink(DNLINK head);

void printDNLink(DNLINK head);

int InsertDNLinkByHead(DNLINK head, DATA data);

#endif