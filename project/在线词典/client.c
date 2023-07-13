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

#define DB "database.db"

char buf[1024] = {0};
typedef struct
{
    char id[20];
    char passwd[20];
} USERLOG;


void _connect();
void menu();
void login(sqlite3* db);
void _register(sqlite3* db);
void loginmenu(sqlite3* db);
int callback(void *data, int argc, char **argv, char **azColName);
sqlite3 *opendb();


int main()
{
    // _connect();
    sqlite3* db = opendb();
    loginmenu(db);
    menu();

    return 0;
}

void _connect()  // 连接服务端
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

sqlite3 *opendb() // 打开数据库模块
{
    int ret;
    sqlite3 *db;
    ret = sqlite3_open(DB, db);
    if (ret)
    {
        fprintf(stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    return db;
}

int callback(void *data, int argc, char **argv, char **azColName) // sqlite 回调
{
    if(argc != 0)
    {
        *(int*)data = 1;
    }
    
    return 0;
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

void login(sqlite3 *db)  // 登录模块
{
    USERLOG user;
    bzero(&user, sizeof(user));
    int flag;
    char *errmsg = NULL;
    char *sql[255];

    while (1)
    {
        flag = 0;
        write(STDOUT_FILENO, "Please enter your user id:", sizeof("Please enter your user id:") - 1);
        fgets(user.id, sizeof(user.id), stdin);
        user.id[strlen(user.id) - 1] = 0;

        write(STDOUT_FILENO, "Please enter password:", sizeof("Please enter password:") - 1);
        fgets(user.passwd, sizeof(user.passwd), stdin);
        user.passwd[strlen(user.passwd) - 1] = 0;

        sprintf(sql, "select id, passwd from user_login where id=%s and passwd=%s", user.id , user.passwd);

        if (sqlite3_exec(db, sql, callback, &flag, &errmsg))
        {
            printf("%s\n", errmsg);
            exit(-1);
        }

        
        if (1 == flag)
        {
            break;
        }
        printf("账号或密码，请重新输入\n");
    }
}

void _register(sqlite3* db)  // 注册模块
{
}

void menu()  // 前台菜单
{
    // while(1)
    // {
    //     printf("");

    // }
}

