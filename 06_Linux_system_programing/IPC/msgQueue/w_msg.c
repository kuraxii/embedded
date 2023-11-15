#include<stdio.h>
#include <sys/ipc.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/msg.h>
#include<stdlib.h>
#define TYPE 100

struct msgbuf{
    long mtype;
    char mtext[BUFSIZ];
};

int main()
{
    int len;
    key_t key;
    struct msgbuf msg;
    if((key = ftok(".", 'a')) < 0)
    {
        perror("ftok");
        exit(0);
    }
    int msgid;
    if((msgid = msgget(key, 0666| IPC_CREAT)) < 0)
    {
        perror("msgerror");
        exit(0);
    }
    printf("opened queue %d\n", msgid);
    puts("Please enter the message to queue\n");
    
    if((fgets(msg.mtext, BUFSIZ, stdin)) == NULL)
    {
        puts("no massage");
        exit(-1);
    }

    msg.mtype = TYPE;
    len = sizeof(msg) - sizeof(long);

    if(msgsnd(msgid, &msg, len, 0) < 0)
    {
        perror("msgsnd");
        exit(-1);
    }


    return 0;
}

