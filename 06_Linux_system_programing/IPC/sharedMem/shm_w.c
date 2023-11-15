#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFZ 4096
int main()
{
    int shmid;
    char *shmbuf;
    if((shmid = shmget(888, BUFZ, 0666|IPC_CREAT)) < 0)
    {
        perror("shmget");
        exit(-1);
    }

    if((shmbuf = shmat(shmid, 0, 0)) == (char*)-1)
    {
        perror("shmat");
        exit(0);
    }
    shmbuf[0] = 0;
    while(1)
    {
        shmbuf[0]++;
        sleep(3);
    }
    
    exit(0);




}
