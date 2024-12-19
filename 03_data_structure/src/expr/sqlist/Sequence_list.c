#include "Sequence_list.h"
#include <malloc.h>
#include <stdlib.h>


sqlist *Createsqlist()
{
    sqlink sq = (sqlink)malloc(sizeof(sqlist));
    if(sq == NULL)
    {   
        perror("malloc:");
        return NULL;
    }
    sq->last = -1;
    return sq;
}

int isEmptySqlist(sqlink sq)  // 判断顺序表是否为空
{
    if(sq == NULL)
    {
        printf("sqlist is NULL\n");
        return -1;
    }

    return sq->last != -1 ? 0 : 1;
}

int isFullsqlist(sqlink sq)  // 判断顺序表是否满 
{
    if(sq == NULL)
    {
        printf("sqlist is NULL\n");
        return -1;
    }
    return sq->last == MAX - 1 ? 1 : 0 ;
}

int Lengthsqlist(sqlink sq)   // 求表长
{
    if(sq == NULL)
    {
        printf("sqlist is NULL\n");
        return -1;
    }
    return (sq->last + 1);
}

void Clearsqlist(sqlink sq)  // 清空顺序表
{
    if(sq == NULL)
    {
        printf("sqlist is NULL\n");
    }
    sq->last = -1;
}

int appendListEnd(sqlink sq, data_t data)
{
    if( sq == NULL )
    {
        printf("sqlist is NULL\n");
        return 0;
    }
    if( isFullsqlist(sq) )
    {
        printf("sqlist is full\n");
        return 0;
    }
    sq->last++;
    sq->data[sq->last] = data;
    return 1;
}

int InsertDataLoc(sqlink sq, int local, data_t data)  // 按位置插入
{
    if( sq == NULL )
    {
        printf("sqlist is NULL\n");
        return 0;
    }
    if( isFullsqlist(sq) )
    {
        printf("sqlist is full\n");
        return 0;
    }
    if(local > sq->last + 1)
    {
        printf("local is too big\n");
        return 0;
    }

    if(local == sq->last + 1){
        sq->data[local] = data;
        sq->last++;
        return 1;
    }
    
    int l = sq->last;
    while(1)
    {
        sq->data[l + 1] = sq->data[l];
        if(local == l)
        {
            sq->data[local] = data;
            break;
        }
        l--;
    }
    sq->last++;
    return 1;
}

int AlterDataLoc(sqlink sq, int local, data_t data)  // 按位置修改
{
    if( isEmptySqlist(sq) )
    {
        printf("sqlist is Empty\n");
        return 0;
    }
    if(local > sq->last)
    {
        printf("local is too big\n");
        return 0;
    }
    
    sq->data[local] = data;
    return 1;
}

int DelDataLoc(sqlink sq, int local)  // 按位置删除
{
    if( isEmptySqlist(sq) )
    {
        printf("sqlist is Empty\n");
        return 0;
    }
   
    if(local < 0 || local > sq->last)
    {
        printf("local is err\n");
        return 0;
    }
    if(local == sq->last)
    {
        sq->last--;
        return 1;
    }

    
    int l = sq->last;
    
    for(int i = local; i < sq->last; i++)
    {
        sq->data[i] = sq->data[i + 1];
    }
    sq->last--;
    return 1;
}

int SeachData(sqlink sq, data_t data)  // 定位元素  返回第一个位置
{
    if( isEmptySqlist(sq) )
    {
        printf("sqlist is Empty\n");
        return 0;
    }
    for(int i = 0; i <= sq->last; i++)
    {
        if(sq->data[i] == data)
        {
            return i;
        }
    }
    return 0;
}

int AlterDataByElement(sqlink sq, data_t data1, data_t data2)  // 按元素修改数据
{
    if( isEmptySqlist(sq) )
    {
        printf("sqlist is Empty\n");
        return 0;
    }

    for(int i = 0; i <= sq->last; i++)
    {
        if(sq->data[i] == data1)
        {
            sq->data[i] = data2;
        }
    }
    return 1;

}


int DelDataByElement(sqlink sq, data_t data)  // 按元素删除数据
{
    if( isEmptySqlist(sq) )
    {
        printf("sqlist is Empty\n");
        return 0;
    }
    for(int i = 0; i <= sq->last; i++)
    {
        if(sq->data[i] == data)
        {
            DelDataLoc(sq, i);
        }
    }
    return 1;
}

void printList(sqlink sq)
{
    if(sq == NULL)
    {
        printf("sqlist is NULL\n");
    }
    for(int i = 0; i <= sq->last; i++)
    {
        printf("%d ", sq->data[i]);
    }
    printf("\n");
}





