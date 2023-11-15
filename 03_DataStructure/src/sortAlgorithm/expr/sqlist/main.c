/*===============================================
*   文件名称：Sequence_list.c
*   创 建 者：Null-zzj
*   创建日期：2023年06月20日
*   描    述：
================================================*/
#include <stdio.h>
#include <unistd.h>
#include "Sequence_list.h"


int main(int argc, char *argv[])
{ 
    sqlist *head = Createsqlist();
    data_t data;
    for(int i = 0; i < 6; i++)
    {
        scanf("%d", &data);
        appendListEnd(head, data);
    }
    printList(head);

    printf("按位置插入\n");
    InsertDataLoc(head, 6, 99);
    printList(head);

    printf("按位置修改\n");
    AlterDataLoc(head, 6, 55);
    printList(head);

    printf("按位置删除\n");
    DelDataLoc(head, 2);
    printList(head);


    printf("定位元素  返回第一个位置\n");
    printf("%d\n", SeachData(head, 2));
    

    printf("按元素修改数据\n");
    AlterDataByElement(head, 2, 66);
    printList(head);

    printf("按元素删除数据\n");
    DelDataByElement(head, 66);
    printList(head);
 
     
    return 0;
} 




