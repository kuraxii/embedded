#include <stdio.h>
#include "dlink.h"

int main()
{

    DNLINK head = CreadeEmptyDNLink();


    InsertDNLinkByHead(head, 5);

    printDNLink(head);

    InsertDNLinkByHead(head, 6);
    InsertDNLinkByHead(head, 7);
    InsertDNLinkByHead(head, 4);

    printDNLink(head);
}



