#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

int main()
{
    struct sembuf p_sb, v_sb;

    int read_semid = semget(128, 1, 0664 | IPC_CREAT);
    int write_semid = semget(129, 1, 0664 | IPC_CREAT);
    
    p_sb.sem_op = -1;
    p_sb.sem_num = 0;
    p_sb.sem_flg = 0;

    v_sb.sem_op = 1;
    v_sb.sem_num = 0;
    v_sb.sem_flg = 0;

    while(1)
    {
        if(semop(write_semid, &p_sb, 1) < 0)
        {
            perror("semop");
        }

        sleep(2);
        printf("write!\n");

        if(semop(read_semid, &v_sb, 1) < 0)
        {
            perror("semop");
        }
    }






    return 0;
}
