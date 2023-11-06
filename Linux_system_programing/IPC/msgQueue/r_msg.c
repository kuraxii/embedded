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
    

    msg.mtype = TYPE;
    len = sizeof(msg) - sizeof(long);

    if(msgrcv(msgid, &msg, len, 0, 0) < 0)
    {
        perror("msgsnd");
        exit(-1);
    }
    printf("recv the msg: %s\n", msg.mtext);
    if(msgctl(msgid, IPC_RMID, NULL) < 0)
    {
        perror("msgctl");
        exit(-1);
    }


    return 0;
}

