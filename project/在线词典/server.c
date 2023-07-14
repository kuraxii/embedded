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
    char id[20];
    char passwd[20];
} USERMSG;
sqlite3 *db;

int login(int cfd);                                                // 登录
int signin(int cfd);                                               // 注册
void opendb();                                                     // 打开数据库
int sockinit(char *ipaddr, unsigned short port);                   // 套接字初始化
void *thread(void *msg);                                           // 客户端处理线程
int callback(void *data, int argc, char **argv, char **azColName); // sqlite回调
void Write(int cfd, char *str);                                    // 写数据封装
void querydic();                                                   // 查字典

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

void *thread(void *msg)
{
    SOCKMSG *p = (SOCKMSG *)msg;
    SOCKMSG climsg;
    climsg.cfd = p->cfd;
    climsg.cli_addr = p->cli_addr;
    climsg.addrlen = p->addrlen;

    int ret;
    char buf[1024] = {0};
    while (1)
    {
        ret = read(climsg.cfd, buf, sizeof(buf));
        printf("recv: %s\n", buf);
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
        if (strncmp(buf, "login", 5) == 0) // 登录
        {
            if ((ret = login(climsg.cfd)) == 0)
            {
                Write(climsg.cfd, "success"); // 登录成功
            }
            else if (ret < 0)
            {
                Write(climsg.cfd, "err"); // 账密错误
            }
            else
            {
                Write(climsg.cfd, "nonexist"); // 账号不存在
            }
        }
        else if (strncmp(buf, "signin", 6) == 0) // 注册
        {
            if (!signin(climsg.cfd))
                Write(climsg.cfd, "exist");

            Write(climsg.cfd, "success");
        }
        else
        { // 输入错误
            write(climsg.cfd, "err", strlen("err"));
        }
    }
}

int login(int cfd)
{
    char buf[64] = {0}, sql[255] = {0}, id[20] = {0}, passwd[20] = {0},
         name[15] = {0}; // 读缓冲区，sql语句，id，passwd，name
    char *ptr = buf;
    char *ptrid = id;
    char **result = NULL;
    int ret, row, col;
    char *errmsg = NULL;

   bzero(buf, sizeof(buf));  
    ret = read(cfd, buf, sizeof(buf));
    buf[ret] = 0;
    strcpy(id, ptr);
    while (*ptr != ' ')
    {
        *ptrid++ = *ptr++;
    }
    *ptrid = 0;
    while (*(++ptr) == ' ');
    strcpy(passwd, ptr);

    printf("-%s-%s-\n", id, passwd);

    sprintf(sql, "select id from user_login where id=\"%s\";", id);
    if (sqlite3_get_table(db, sql, &result, &row, &col, &errmsg) != SQLITE_OK) // 查表检测用户是否已存在
    {
        printf("%s\n", errmsg);
        exit(-1);
    }
    printf("row= %d, col= %d\n%p\n", row, col, result);

    if (row == 0) // 用户不存在
    {  
        return 1; // 不存在
    }

    // 用户存在，检查密码是否正确
    if (strcmp(result[1 * col], passwd) == 0) // 正确
    { 
       return 0; // 账密正确
    }
    else
    { 
        return -1; // 账密错误
   }
    
}

int signin(int cfd) // 注册
{

    char buf[64] = {0}, sql[255] = {0}, id[20] = {0}, passwd[20] = {0},
         name[15] = {0}; // 读缓冲区，sql语句，id，passwd，name
    char *ptr = buf;

    char **result;
    int ret, row, col;
    char *errmsg;

    read(cfd, buf, sizeof(buf));
    strcpy(id, ptr);
    while (*(++ptr) == 0)
        ;
    strcpy(passwd, ptr);

    sprintf(sql, "select id from user_login where id=\"%s\";", id);
    if (sqlite3_get_table(db, sql, &result, &row, &col, &errmsg) != SQLITE_OK) // 查表检测用户是否已存在
    {
        printf("sqlite3_get_table: %s\n", errmsg);
        exit(-1);
    }

    if (row != 0) // 用户存在，向客户端发送已存在的信息
    {
        return -1;
    }

    // 用户不存在，插入表
    sprintf(sql, "insert into user_login values(\"%s\", \"%s\", \"%s\", %d)", id, passwd, name, 0);
    if (sqlite3_get_table(db, sql, NULL, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("sqlite3_get_table: %s\n", errmsg);
        exit(-1);
    }

    return 0;
}

void querydic() // 查字典
{
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
    if(ret == -1)
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

int callback(void *data, int argc, char **argv, char **azColName) // sqlite 回调
{
    if (argc != 0)
    {
        *(int *)data = 1;
    }

    return 0;
}

void Write(int cfd, char *str)
{
    int ret;
    ret = write(cfd, str, strlen(str));
    if (ret != strlen(str))
    {
        perror("write");
        exit(-1);
    }
}