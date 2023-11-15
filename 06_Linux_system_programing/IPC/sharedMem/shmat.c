#include <myinclude.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int shmid;
    char* shmbuf;

    if(argc != 2)
    {
        puts("USAGE: shmid\n");
        exit(0);
    }

    shmid = atoi(argv[1]);

    if((shmbuf = shmat(shmid, 0 ,0)) == (char*)-1)
    {
        perror("shmat");
        exit(-1);
    }
    printf("segment attached at %p\n", shmbuf);


    sleep(20);
    if((shmdt(shmbuf)) < 0)
    {
        perror("shmdt");
        exit(-1);
    }

    puts("segment detached");

    exit(0);
}
