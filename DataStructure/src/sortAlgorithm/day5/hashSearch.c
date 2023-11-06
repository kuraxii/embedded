#include <stdio.h>

int *initHash(int m )
{
    int *h = (int*)malloc(sizeof(int) * m);
    for(int i = 0; i < m; i ++)
        h[i] = -1;
    return h;
}

int insertHash(int *hash, int *buf, int n, int m)
{
    int p = func(m);
    for(int i = 0;i < n; i++)
    {
        int hash_val = buf[i] % p;
        while(hash[hash_val] != 0)
        {
            hash_val = (hash_val + 1) % m;
        }

        hash[hash_val] = buf[i];

    }

}

int main()
{



}