#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<semaphore.h>
#include<sys/types.h>
#include<fcntl.h>

int main()
{
    sem_t *r_sem, *w_sem;


    if((r_sem = sem_open("r_sem", O_CREAT, 0600, 0)) < 0)
    {
        perror("sem_open");
        exit(-1);
    }
    if((w_sem = sem_open("w_sem", O_CREAT, 0600, 1)) < 0)
    {
        perror("sem_open");
        exit(-1);
    }

    while(1)
    {
        if(sem_wait(w_sem) < 0)
        {
            perror("sem_wait");
            exit(-1);
        }
        sleep(2);
        printf("write!\n");
        if(sem_post(r_sem) < 0)
        {
            perror("sem_post");
            exit(-1);
        }
    }
    sem_close(r_sem);
    sem_close(w_sem);
    return 0;
}
