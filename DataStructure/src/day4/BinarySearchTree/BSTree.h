/*===============================================
*   文件名称：btree.h
*   创 建 者：Null-zzj
*   创建日期：2023年06月26日
*   描    述：
================================================*/
#ifndef _BSTREE_H
#define _BSTREE_H

typedef int Data;


typedef struct node{

struct node *lchild, *rchild;
Data data;

}BSTree;

void insertBSTree(BSTree** root, Data data);
BSTree* searchBSTree(BSTree* root, Data data);
void MidOrderTraverse(BSTree* T);
#endif


