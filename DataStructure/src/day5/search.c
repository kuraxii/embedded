#include <stdio.h>


int search(int* arr, int num, int l, int r)
{
    if(l > r)
        return -1;

    int mid = (l + r) / 2;

    if(num == arr[mid])  return 1;

    if(arr[mid] > num)  return search(arr, num, l, mid - 1);
    else  return search(arr, num, mid + 1, r);


}

int main()
{

    int arr[1000] = {0};
    for(int i = 0; i < 1000; i++)
    {
        arr[i] = i;
    }

    int key;

    while(scanf("%d", &key))
    {
        if(search(arr, key, 0, sizeof(arr)/ sizeof(arr[0]) - 1) != -1)
            printf("数字%d在数组中存在\n",key);
        else
            printf("数字%d在数组中不存在\n",key);

        // printf("%d\n", search(arr, key, 0, sizeof(arr)/ sizeof(arr[0]) - 1));
    }
}
