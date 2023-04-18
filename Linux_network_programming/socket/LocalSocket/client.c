#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <stddef.h>
void sys_err(char *str)
{
    perror(str);
    exit(1);
}
#define SERV_ADDR "serv.socket"
#define CLI_ADDR "cli.sock"
int main(int argc,char *argv[])
{
    int cfd, ser_fd;
    char buf[BUFSIZ];
    memset(buf, '\0', sizeof(buf));
    cfd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un ser_addr, cli_addr;
   
    memset(&ser_addr, 0, sizeof(ser_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));

    cli_addr.sun_family = AF_UNIX;
    strcpy(cli_addr.sun_path, CLI_ADDR);
    int len = offsetof(struct sockaddr_un, sun_path) + strlen(cli_addr.sun_path);

    unlink(CLI_ADDR);
    bind(cfd, (struct sockaddr*)&cli_addr, len);


    ser_addr.sun_family = AF_UNIX;
    strcpy(ser_addr.sun_path, SERV_ADDR);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(ser_addr.sun_path);
    connect(cfd, (struct sockaddr*)&ser_addr, len);
    printf("client connect success\n");

    while (1)
    {
        scanf("%s", buf);
        write(cfd, buf, strlen(buf));
        len = read(cfd, buf, sizeof(buf));
        write(STDOUT_FILENO, buf, len);
    }

    close(cfd);
    close(ser_fd);
    



    return 0;
}