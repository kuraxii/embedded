#include <malloc.h>
#include "BSTree.h"




void insertBSTree(BSTree** root, Data data)
{
    if(*root == NULL)
    {
        *root = malloc(sizeof(BSTree));
        (*root)->data = data;
        (*root)->lchild = NULL;
        (*root)->rchild = NULL;

    }

    else{
        BSTree* curr = *root;
        BSTree* entry;
        while(curr)
        {
            entry = curr;
            if(curr->data == data)
                return;

            if(data < curr->data)
                curr = curr->lchild;
            else
                curr = curr->rchild;
            

        }

        BSTree* node = malloc(sizeof(BSTree));
        node->data = data;
        node->lchild = NULL;
        node->rchild = NULL;

        if(data > entry->data)
        {
            entry->rchild = node;
        }else
        {
            entry->lchild = node;
        }
    }

}

void MidOrderTraverse(BSTree* T) {
    if (!T)
        return;

    MidOrderTraverse(T->lchild);
    printf("%d ", T->data);  
    MidOrderTraverse(T->rchild);
}
BSTree* searchBSTree(BSTree* root, Data data)
{
    if(root->data == data || root == NULL)
        return root;

    if(root->data < data) return searchBSTree(root->lchild, data);
    if(root->data > data) return searchBSTree(root->rchild, data);

}

