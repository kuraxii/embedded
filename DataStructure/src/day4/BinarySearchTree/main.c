#include <stdio.h>
#include "BSTree.h"
#include <time.h>
#include <stdlib.h>
int main()
{
    BSTree* root = NULL;
    srand((unsigned)time(NULL));

    for(int i = 0; i < 10; i++)
        insertBSTree(&root, rand() % 50 + 1);
    
    // insertBSTree(&root, 6);
    // insertBSTree(&root, 8);
    // insertBSTree(&root, 19);
    // insertBSTree(&root, 7);
    // insertBSTree(&root, 1);
    // insertBSTree(&root, 3);
    // insertBSTree(&root, 67);
    // insertBSTree(&root, 33);
    // insertBSTree(&root, 22);
    // insertBSTree(&root, 55);

    MidOrderTraverse(root);
    printf("\n");

}