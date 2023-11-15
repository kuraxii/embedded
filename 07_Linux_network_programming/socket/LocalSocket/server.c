#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <ctype.h>

void sys_err(char *str)
{
    perror(str);
    exit(1);
}

#define SERV_ADDR "serv.socket"

int main(int argc,char *argv[])
{
    int ret, lfd, cfd;
    char buf[BUFSIZ];
    memset(buf, '\0', sizeof(buf));
    lfd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un ser_addr, cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr);
    memset(&ser_addr, 0, sizeof(ser_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));
    ser_addr.sun_family = AF_UNIX;
    strcpy(ser_addr.sun_path, SERV_ADDR);
    int len = offsetof(struct sockaddr_un, sun_path) + strlen(ser_addr.sun_path);
    unlink(SERV_ADDR);
    bind(lfd, (struct sockaddr*)&ser_addr, len);

    listen(lfd, 255);

    cfd = accept(lfd, (struct sockaddr*)&cli_addr, (socklen_t*)&len);
    len -= offsetof(struct sockaddr_un, sun_path);
    cli_addr.sun_path[len] = '\0'; 
    printf("client connect success path = %s\n", cli_addr.sun_path);
    int size, i;
    while((size = read(cfd, buf, sizeof(buf))) > 0 ){
        for(i = 0; i < size; i ++){
            buf[i] = toupper(buf[i]);
        }
        write(cfd, buf, size);
    }

    close(cfd);
    close(lfd);




    





    return 0;
}