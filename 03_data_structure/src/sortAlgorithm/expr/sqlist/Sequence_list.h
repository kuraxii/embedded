/*===============================================
*   文件名称：Sequence_list.h
*   创 建 者：Null-zzj
*   创建日期：2023年06月20日
*   描    述：
================================================*/
#ifndef _SEQUENCE_LIST_H
#define _SEQUENCE_LIST_H
#define MAX 10
typedef int data_t;

typedef struct{
    data_t data[MAX];
    int last;
}sqlist, *sqlink;

sqlist *Createsqlist();
int isEmptySqlist(sqlink sq);  // 判断顺序表是否为空
int isFullsqlist(sqlink sq);   // 判断是否满
int Lengthsqlist(sqlink sq);   // 求表长
void Clearsqlist(sqlink sq);  // 清空顺序表
int appendListEnd(sqlink sq, data_t data);  // 在末尾插入
int InsertDataLoc(sqlink sq, int local, data_t data);   // 按位置插入
int AlterDataLoc(sqlink sq, int local, data_t data);  // 按位置修改
int DelDataLoc(sqlink sq, int local);  // 按位置删除
int SeachData(sqlink sq, data_t data);  // 定位元素  返回第一个位置
int AlterDataByElement(sqlink sq, data_t data1, data_t data2);  // 按元素修改数据
int DelDataByElement(sqlink sq, data_t data);  // 按元素删除数据
void printList(sqlink sq);   // 打印
#endif
