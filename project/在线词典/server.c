#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <ctype.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define DB "database.db"
#define PORT 9999

typedef struct
{
    int cfd;
    struct sockaddr_in cli_addr;
    socklen_t addrlen;
} SOCKMSG;

typedef struct
{
    char type;
    char id[20];
    char name[20];
    char data[128];
} USERMSG;

sqlite3 *db;

void WriteCond(int cfd, char c);                                          // 写状态封装
void Write(int cfd, USERMSG msg);                                         // 写数据报封装
USERMSG login(int cfd, USERMSG msg);                                      // 登录
int signin(int cfd, USERMSG msg);                                         // 注册
void opendb();                                                            // 打开数据库
int sockinit(char *ipaddr, unsigned short port);                          // 套接字初始化
void *thread(void *sockmsg);                                              // 客户端处理线程
int historycallback(void *data, int argc, char **argv, char **azColName); // sqlite回调
void querydic(SOCKMSG csock, USERMSG cond);                               // 查字典
void history(SOCKMSG csock, USERMSG cond);                                // 查历史记录

int main(int argc, char *argv[])
{
    int ret;
    pthread_t pid;
    opendb();
    int lfd = sockinit(NULL, PORT);

    SOCKMSG cli_msg;
    bzero(&cli_msg, sizeof(cli_msg));
    cli_msg.addrlen = sizeof(cli_msg.addrlen);

    while (1)
    {
        int cfd = accept(lfd, (struct sockaddr *)&cli_msg.cli_addr, &cli_msg.addrlen);
        if (cfd == -1)
        {
            perror("accept");
            exit(-1);
        }
        cli_msg.cfd = cfd;

        printf("client ip: %s, port: %d connect success\n", inet_ntoa(cli_msg.cli_addr.sin_addr),
               ntohs(cli_msg.cli_addr.sin_port));

        ret = pthread_create(&pid, NULL, thread, (void *)&cli_msg);
        if (ret != 0)
        {
            printf("%s\n", strerror(ret));
            exit(-1);
        }
        ret = pthread_detach(pid);
        if (ret != 0)
        {
            printf("%s\n", strerror(ret));
            exit(-1);
        }
    }
    pthread_exit(NULL);
}

void *thread(void *sockmsg)
{
    SOCKMSG *p = (SOCKMSG *)sockmsg;
    SOCKMSG climsg;
    USERMSG usermsg, cond;
    climsg.cfd = p->cfd;
    climsg.cli_addr = p->cli_addr;
    climsg.addrlen = p->addrlen;

    int ret;

    char buf[1024] = {0};
    while (1)
    {
        printf("menu\n");
        ret = read(climsg.cfd, &usermsg, sizeof(usermsg));
        if (ret == -1)
        {
            perror("read");
            pthread_exit(NULL);
        }
        if (ret == 0)
        {
            printf("client ip: %s, port: %d disconnect\n", inet_ntoa(climsg.cli_addr.sin_addr),
                   ntohs(climsg.cli_addr.sin_port));
            close(climsg.cfd);
            pthread_exit(NULL);
        }

        if (ret != sizeof(usermsg))
        {
            printf("read err: ret != sizeof(usermsg)\n");
            pthread_exit(NULL);
        }

        switch (usermsg.type)
        {
        case 'L': {
            if ((cond = login(climsg.cfd, usermsg)).type == 0)
            {

                printf("登录成功\n");
                while (1)
                {
                    read(climsg.cfd, &usermsg, sizeof(usermsg));
                    printf("recv: %c\n", usermsg.type);
                    switch (usermsg.type)
                    {
                    case 'Q':
                        querydic(climsg, cond);
                        break;
                    case 'H':
                        history(climsg, cond);
                        break;
                    case 'O':
                        goto exit;
                    default:
                        WriteCond(climsg.cfd, 'i');
                    }
                }
            }
            else if (cond.type < 0)
            {

                printf("账密错误\n");
            }
            else
            {
                // 账号不存在
                printf("账号不存在\n");
            }
        }
        exit:
        break;

        case 'S': {
            if (!signin(climsg.cfd, usermsg))
            {
                printf("成功\n");
            }
            else
                printf("用户存在\n");
        }
        break;
        default:
            WriteCond(climsg.cfd, 'i'); // 输入错误
        }
    }
}

USERMSG login(int cfd, USERMSG msg)
{
    printf("login\n");
    char sql[255] = {0};
    int ret, row, col;
    char **result = NULL;
    char *errmsg = NULL;
    USERMSG cond;

    sprintf(sql, "select id, passwd, name from user_login where id=\"%s\";", msg.id);
    if (sqlite3_get_table(db, sql, &result, &row, &col, &errmsg) != SQLITE_OK) // 查表检测用户是否已存在
    {
        printf("%s\n", errmsg);
        pthread_exit(NULL);
    }

    if (row == 0) // 用户不存在
    {
        cond.type = 'n';
        Write(cfd, cond);
        cond.type = 1;
        return cond; // 不存在
    }

    // 用户存在，检查密码是否正确
    if (strcmp(result[1 * col + 1], msg.data) == 0) // 正确
    {
        cond.type = 't';
        strcpy(cond.id, result[1 * col]);
        strcpy(cond.name, result[1 * col + 2]);
        Write(cfd, cond); // 登录成功
        cond.type = 0;
        return cond; // 账密正确
    }
    else
    {
        cond.type = 'f';
        Write(cfd, cond); // 账密错误
        cond.type = -1;
        return cond; // 账密错误
    }
}

int signin(int cfd, USERMSG msg) // 注册
{
    printf("signin\n");
    char sql[255] = {0};
    int ret = 0, row = 0, col = 0;
    char **result = NULL;
    char *errmsg = NULL;

    sprintf(sql, "select id from user_login where id=\"%s\";", msg.id);
    if (sqlite3_get_table(db, sql, &result, &row, &col, &errmsg) != SQLITE_OK) // 查表检测用户是否已存在
    {
        printf("sqlite3_get_table: %s\n", errmsg);
        pthread_exit(NULL);
    }

    if (row != 0) // 用户存在，向客户端发送已存在的信息
    {
        printf("账号存在\n");
        WriteCond(cfd, 'e'); // 账号存在
        return -1;
    }

    // 用户不存在，插入表
    sprintf(sql, "insert into user_login values(\"%s\", \"%s\", \"%s\")", msg.id, msg.data, msg.name);

    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("sqlite3_exec: %s\n", errmsg);
        pthread_exit(NULL);
    }

    WriteCond(cfd, 't'); // 成功
    return 0;
}

void querydic(SOCKMSG csock, USERMSG cond) // 查字典
{

    char sql[255] = {0};
    int ret, row, col;
    char **result = NULL;
    char *errmsg = NULL, *_time;
    USERMSG msg;
    USERMSG buf;
    while (1)
    {
        printf("查字典\n");
        ret = read(csock.cfd, &buf, sizeof(buf));
        if (ret == -1)
        {
            perror("read");
            pthread_exit(NULL);
        }
        if (ret == 0)
        {
            printf("client ip: %s, port: %d disconnect\n", inet_ntoa(csock.cli_addr.sin_addr),
                   ntohs(csock.cli_addr.sin_port));
            pthread_exit(NULL);
        }
        else if (ret != sizeof(USERMSG))
        {
            printf("translate error\n");
            pthread_exit(NULL);
        }
        if (buf.type != 'Q')
        {
            break;
        }
        else
        {
            
            sprintf(sql, "select * from dict where word=\"%s\";", buf.data);
            if (sqlite3_get_table(db, sql, &result, &row, &col, &errmsg) != SQLITE_OK)
            {
                printf("sqlite3_get_table: %s\n", errmsg);
                pthread_exit(NULL);

            }

            time_t tim = time(NULL);
            _time = ctime(&tim);
            _time[strlen(_time) - 1] = 0;

            sprintf(sql, "insert into history values(\"%s\",\"%s\",\"%s\",\"%s\");", cond.id, cond.name, buf.data,
                    _time);
            if (sqlite3_exec(db, sql, NULL, NULL, &errmsg))
            {
                printf("sqlite3_exec: %s\n", errmsg);
                pthread_exit(NULL);
            }

            if (row != 0) // 查到词典
            {
                printf("result: %s\n", result[col * 1 + 1]);
                msg.type = 'q';
                strcpy(msg.data, result[col * 1 + 1]);
                Write(csock.cfd, msg);
            }else {
                msg.type = 'q';
                strcpy(msg.data, "查无该字");
                Write(csock.cfd, msg);
            }
        }
    }
}

void history(SOCKMSG csock, USERMSG cond)
{
    char sql[255] = {0};
    int row, col;

    char **result = NULL;

    char *errmsg = NULL;
    USERMSG msg;

    sprintf(sql, "select * from history where id=\"%s\";", cond.id);

   

    if(sqlite3_exec(db, sql, historycallback, &csock.cfd, &errmsg))
    {
        if (errmsg != NULL)
        {
            printf("sqlite3_exec: %s\n", errmsg);
            sqlite3_free(errmsg); // 释放错误消息的内存
            return;
        }
    }

    msg.type = 'o';
    Write(csock.cfd, msg);
    printf("history ok!\n");
}

int sockinit(char *ipaddr, unsigned short port) // 初始化服务端套接字
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        return -1;
    }

    int opt = 1, ret;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (ret == -1)
    {
        perror("setsockopt");
        exit(-1);
    }

    struct sockaddr_in seraddr;
    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_addr.s_addr = INADDR_ANY;
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(port);
    ret = bind(lfd, (struct sockaddr *)&seraddr, sizeof(seraddr));
    if (ret == -1)
    {
        perror("bind");
        exit(-1);
    }
    ret = listen(lfd, 255);
    if (ret == -1)
    {
        perror("listen");
        exit(-1);
    }
    return lfd;
}

void opendb() // 打开数据库模块
{
    int ret;

    ret = sqlite3_open(DB, &db);
    if (ret)
    {
        fprintf(stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db));
        exit(-1);
    }
}

int historycallback(void *data, int argc, char **argv, char **azColName) // sqlite 回调
{
    int cfd = *(int *)data;
    USERMSG msg;
    msg.type = 'h';
    sprintf(msg.data, "%-10s%-10s%-10s%s", argv[0], argv[1], argv[2], argv[3]);
    Write(cfd, msg);

    return 0;
}

void Write(int cfd, USERMSG msg)
{
    int ret;
    ret = write(cfd, &msg, sizeof(msg));
    if (ret != sizeof(msg))
    {
        printf("translate error\n");
        pthread_exit(NULL);
    }
}

void WriteCond(int cfd, char c)
{
    int ret;
    USERMSG msg;
    bzero(&msg, sizeof(msg));
    msg.type = c;

    ret = write(cfd, &msg, sizeof(msg));
    if (ret != sizeof(msg))
    {
        perror("write");
        pthread_exit(NULL);
    }
}
