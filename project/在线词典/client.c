#include <math.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include <sqlite3.h>
#include "sql.h"
#include "user.h"


char buf[1024] = {0};



int _connect();
void menu();

void loginmenu(sqlite3* db);



int main()
{
    // int cfd = _connect();
    sqlite3* db = opendb();
    loginmenu(db);
    menu();

    return 0;
}

int _connect()  // 连接服务端
{
    int cfd;
    cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd == -1)
    {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in cliaddr;
    cliaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    cliaddr.sin_port = htons(9999);
    cliaddr.sin_family = AF_INET;

    int ret = connect(cfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
    if (ret == -1)
    {
        perror("connect");
        exit(-1);
    }
    return cfd;
}



void loginmenu(sqlite3* db)  // 登录菜单
{
    int i;
    printf("******请选择********\n"
           "1------登录---------\n"
           "2------注册---------\n"
           "*******************");
    scanf("%d", &i);
    i == 1 ? login(db) : _register(db);
}





void menu()  // 前台菜单
{
    // while(1)
    // {
    //     printf("");

    // }
}

