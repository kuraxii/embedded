#include<myinclude.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include <sys/types.h>
#include<stdlib.h>
int main()
{
    int shmid;
    if((shmid = shmget(555, 4096, 0666| IPC_CREAT)) < 0)
    {
        perror("shmget");
        exit(0);
    }

    printf("segment creat");
    pause();

    pause();



}
