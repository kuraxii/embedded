#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>
void sys_err(char *str)
{
    perror(str);
    exit(1);
}

int main(int argc,char *argv[])
{
    int sockfd, ret;
    char buf[BUFSIZ];
    memset(buf, '\0', sizeof(buf));
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in ser_addr;
    socklen_t ser_addr_len = sizeof(ser_addr); 
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(12500);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);


    while(1){
        scanf("%s", buf);
        sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&ser_addr, ser_addr_len);


        int len = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
        if(len > 0){
            printf("%s\n",buf);
        }

    }

    bzero("x", 5);

    return 0;
}