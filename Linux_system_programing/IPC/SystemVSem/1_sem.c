#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/types.h>
int main()
{
    int semid;
    struct sembuf sb;
    
    
    semid = semget(1024, 5, 0666 | IPC_CREAT);
    

    
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    
    

    printf("will begin P operation\n");
    semop(semid, &sb, 1);
    printf("yang\n");
    return 0;

}
