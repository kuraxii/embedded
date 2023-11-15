#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
void sys_err(char *str)
{
    perror(str);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, ret;
    char buf[BUFSIZ];
    memset(buf, '\0', sizeof(buf));
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in ser_addr, cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr); 

    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(12500);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd, (struct sockaddr* )&ser_addr, sizeof(ser_addr));

    

    while(1){
        int len = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&cli_addr, &cli_addr_len);
        if(len < 0){
            sys_err("recvfrom err");
        }
        if(len == 0){
            printf("client ip = %s close\n", inet_ntoa(cli_addr.sin_addr));
        }
        if(len > 0){
            for (size_t i = 0; i < len; i++)
            {
                buf[i] = toupper(buf[i]);
            }
            sendto(sockfd, buf, len, 0, (struct sockaddr*)&cli_addr, cli_addr_len);
            
        }
    }

    

    return 0;
}