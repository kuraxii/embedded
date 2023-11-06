#include <malloc.h>
#include "btree.h"

Btree *createNode(int n, int i)
{
    Btree *node = malloc(sizeof(Btree));
    if (node == NULL)
    {
        perror("malloc");
        return NULL;
    }
    node->data = i;
    if (i * 2 <= n)
    {
        node->lchild = createNode(n, i * 2);
    }
    else
    {
        node->lchild = NULL;
    }

    if (i * 2 + 1 <= n)
    {
        node->rchild = createNode(n, i * 2 + 1);
    }
    else
    {
        node->rchild = NULL;
    }

    return node;
}

void PreOrderTraverse(Btree *T) // 先序
{ 
    if (!T)
        return;

    printf("%d ", T->data);
    PreOrderTraverse(T->lchild);
    PreOrderTraverse(T->rchild);
}

void _PreOrderTraverse(Btree *T) // 迭代法先序
{
    Btree* stack[1024] = {0};
    int rear = 0, top = -1;
    
    Btree* curr = T;
    stack[++top] = curr;
    while(top >= 0)
    {
        curr = stack[top--];
        printf("%d ", curr->data);
         
        if(curr->rchild)
            stack[++top] = curr->rchild;
        if(curr->lchild)
            stack[++top] = curr->lchild;
    }

}

void MidOrderTraverse(Btree *T)  // 中序
{
    if (!T)
        return;

    MidOrderTraverse(T->lchild);
    printf("%d ", T->data);
    MidOrderTraverse(T->rchild);
}

void _MidOrderTraverse(Btree *T)  // 迭代法中序   访问顺序  左中右
{
    Btree* stack[1024] = {0};
    int rear = 0, top = -1;
    Btree* curr = T;
    while(curr || top >= 0)
    {
        if(curr)   // 父节点入栈
        {
            stack[++top] = curr;   
            curr = curr->lchild;   // 指向左子节点，左节点优先入栈
            
        }else   // 如果为空，说明该节点无左右子树，则应该出栈
        {
            curr = stack[top--];    // 出栈
            printf("%d ", curr->data);
            curr = curr->rchild;     // 右节点后入栈，如果右节点还是为空，则再次出栈
        }
    }
}

void AftOrderTraverse(Btree *T)  // 后序
{
    if (!T)
        return;

    AftOrderTraverse(T->lchild);
    AftOrderTraverse(T->rchild);
    printf("%d ", T->data);
}

void _AftOrderTraverse(Btree *T) // 迭代法后序
 {


      
}


// 层序遍历
void levelOrder(Btree *T)
{
    Btree* queue[1024] = {0};  // 声明队列
    int rear = 0, front = 0;
    Btree* curr = T;

    queue[rear] = curr;      // root节点入队
    rear = (rear + 1) % 1024;
    while(front != rear)
    {
        curr = queue[front];        // 先将队列中的第一个节点出队，再将子节点入队，形成有序序列
        printf("%d ", curr->data);
        front = (front + 1) % 1024;
        

        if(curr->lchild != NULL)
        {
            queue[rear] = curr->lchild;
            rear = (rear + 1) % 1024;
        }
        if(curr->rchild != NULL)
        {
            queue[rear] = curr->rchild;
            rear = (rear + 1) % 1024;
        }

    }

}