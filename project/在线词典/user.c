#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "sql.h"
#include "user.h"

void login(sqlite3 *db)  // 登录模块
{
    USERLOG user;
    bzero(&user, sizeof(user));
    int flag;
    char *errmsg = NULL;
    char sql[255];

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
    USERLOG user;
    bzero(&user, sizeof(user));
    int flag;
    char *errmsg = NULL;
    char sql[255];

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