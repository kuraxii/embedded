/*===============================================
*   文件名称：main.c
*   创 建 者：Null-zzj
*   创建日期：2023年06月26日
*   描    述：
================================================*/
#include <stdio.h>
#include <unistd.h>
#include "btree.h"
int main(int argc, char *argv[])
{ 
    printf("创建完全二叉树\n");
    Btree* btree = createNode(10, 1);
    printf("前序遍历\n");
    PreOrderTraverse(btree);
    printf("\n");

    printf("迭代法前序遍历\n");
    _PreOrderTraverse(btree);
    printf("\n");
   
    printf("中序遍历\n");
    MidOrderTraverse(btree);
    printf("\n");

    printf("迭代法中序遍历\n");
    _MidOrderTraverse(btree);
    printf("\n");

    printf("后序遍历\n");
    AftOrderTraverse(btree);
    printf("\n");

    printf("层序遍历\n");
    levelOrder(btree);
    printf("\n");



    return 0;
} 
