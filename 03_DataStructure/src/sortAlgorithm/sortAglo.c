/*
 * @Filename: shellsort.c
 * @Author: Null-zzj
 * @Date: 23-06-22 18:57
 * @Descript:
 */

#include <stdio.h>

void shellsort(int *arr, int len)   // 希尔排序
{
    int i, j;
    int increment = len;
    int tmp;
    while(increment > 1)
    {
        increment = increment / 2;   // 设置间隔分组
        
        for(i = increment; i < len ; i ++)
        {
            if(arr[i - increment] > arr[i])
            {
                tmp = arr[i];
                for(j = i - increment; j >= 0 && arr[j] > tmp; j -= increment)   // 查找插入位置
                {
                    arr[j + increment] = arr[j];      // 向前插入  ==> 向后移动
                }
                
                arr[j + increment] = tmp;

            }
        }
    }
   
}

void swap(int* n, int* m)
{
    *n += *m;
    *m = *n - *m;
    *n = *n - *m;
}

void headAdjudge(int *arr, int i, int length)
{
    int tmp = arr[i];
    int j;
    for(j = i * 2 + 1; j < length; j = j * 2 + 1)
    {
        if(j + 1 < length && arr[j] < arr[j + 1])   // 标记出最大子节点的位置
        {
            j++;
        }
        if(tmp > arr[j])   // 当前值大于子节点最大值，则退出循环，当前位置为应插入位置. 否则交换
            break;        
        arr[i] = arr[j];
        i = j;     // i 记录当j的位置
    }
    arr[i] = tmp;

}


int* heapsort(int *arr, int len)  // 堆排
{
    for(int i = len / 2 - 1; i >= 0; i--)
    {
        headAdjudge(arr, i, len);        //  构造大顶堆
    }

   for(int i = len - 1; i > 0; i--)     // 将顶移到最后
   {
        swap(&arr[0], &arr[i]);
        headAdjudge(arr, 0, i);
   }
}

void merger(int* arr, int* mer, int left, int right, int mid)
{
    int i = left, j = mid;
    int p = left;

    while(i < mid && j < right)
    {
        if(arr[i] < arr[j])
            mer[p++] = arr[i++];
        else
            mer[p++] = arr[j++];
    }

    while(i < mid)
        mer[p++] = arr[i++];

    while(j < right)
        mer[p++] = arr[j++];

    for(i = left; i < right; i++)
        arr[i] = mer[i];
    

}

void msort(int* arr, int* mer, int left, int right)
{
    if(left >= right)
        return; 

    int mid = (left + right) / 2;
    
    msort(arr, mer, left, mid);    // 区间为左闭右开
    msort(arr, mer, mid, right);
    merger(arr, mer, left, right, mid);

}

void megersort(int *arr, int length)    // 归并
{
    int mer[length];
    msort(arr, mer, 0, length);    
}




void quickSort(int *arr, int left, int right)
{
    if(left >= right)
        return;
    
    int i = left - 1, j = right + 1;
    int tmp = arr[(left + right) / 2];
    while(i < j)
    {
        do i++; while(arr[i] < tmp);
        do j--; while(arr[j] > tmp);

        if(i < j) swap(&arr[i], &arr[j]);   // 排序完成后，j指向第一个小于基准元素的地址
    }

    quicksort(arr, left, j);
    quicksort(arr, j + 1, right);

}



int main(int argc, char *argv[])
{
    // int arr[] = {3,2,1,6,5,4,9,8};
    // shellsort(arr, sizeof(arr)/sizeof(arr[0]));
    // for(int i = 0; i < sizeof(arr)/sizeof(arr[0]); i ++)gcc
    // {
    //     printf("%d ", arr[i]);
    // }
    // printf("\n");


    // int arr[] = {3,2,1,6,5,4,9,8};
    // heapsort(arr, sizeof(arr)/sizeof(arr[0]));
    // for(int i = 0; i < sizeof(arr)/sizeof(arr[0]); i ++)
    // {
    //     printf("%d ", arr[i]);
    // }
    // printf("\n");


    // int arr[] = {9,2,1,6,5,4,3,8};
    // quickSort(arr, 0, sizeof(arr)/sizeof(arr[0]) - 1);
    // for(int i = 0; i < sizeof(arr)/sizeof(arr[0]); i ++)
    // {
    //     printf("%d ", arr[i]);
    // }
    // printf("\n");

    int arr[] = {3,2,1,6,5,4,9,8};
    megersort(arr, sizeof(arr)/sizeof(arr[0]));

    for(int i = 0; i < sizeof(arr)/sizeof(arr[0]); i ++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}



