/*===============================================
*   文件名称：btree.h
*   创 建 者：Null-zzj
*   创建日期：2023年06月26日
*   描    述：
================================================*/

#ifndef _BTREE_H
#define _BTREE_H
typedef int Data;

typedef struct node
{
    Data data;
    struct node *rchild, *lchild;
}Btree;

Btree* createNode(int n, int i);   // 完全二叉树
void PreOrderTraverse(Btree* T);   // 前序遍历
void _PreOrderTraverse(Btree *T); // 迭代法先序
void MidOrderTraverse(Btree* T);   // 中序遍历
void _MidOrderTraverse(Btree *T);  // 迭代法中序
void AftOrderTraverse(Btree* T);   // 后序遍历
void levelOrder(Btree *T);

#endif
