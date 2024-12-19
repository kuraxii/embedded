#include <stdio.h>

void swap(int *a, int *b)
{
    *a = *a + *b;
    *b = *a - *b;
    *a = *a - *b;
}

void quickSort(int *arr, int left, int right)
{
    if (left >= right)
        return;
    int tmp = arr[(left + right) / 2];

    int i = left - 1, j = right + 1;
    while (i < j)
    {
        do i++; while (arr[i] < tmp);
        do j--; while (arr[j] > tmp);

        if (i < j)
            swap(&arr[i], &arr[j]);
    }
    quickSort(arr, left, j);
    quickSort(arr, j + 1, right);
}

int main()
{
    int arr[] = {9,2,1,6,5,4,3,8};
    quickSort(arr, 0, sizeof(arr)/sizeof(arr[0]) - 1);
    for(int i = 0; i < sizeof(arr)/sizeof(arr[0]); i ++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");

}