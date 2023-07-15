#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

char buf[1024] = {0};
typedef struct
{
    char type;
    char id[20];
    char name[20];
    char data[128];
} USERMSG;

int _connect();
int menu(int cfd, USERMSG msg);
void loginmenu(int cfd);
int signin(int cfd);
int login(int cfd);
void query(); // 查询
void history();

int main()
{
    int cfd = _connect();

    loginmenu(cfd);

    return 0;
}

int _connect() // 连接服务端
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

void loginmenu(int cfd) // 登录菜单
{
    int i;

    while (1)
    {
        system("clear");
        printf("******请选择********\n"
               "1------登录---------\n"
               "2------注册---------\n"
               "3------退出---------\n"
               "*******************\n> ");
        scanf("%d", &i);
        switch (i)
        {
        case 1:
            login(cfd);
            break;
        case 2:
            if(signin(cfd) == 0)
            {
                printf("注册成功!\n");
                goto exit;
            }
            break;
        case 3:
            goto exit;
            break;
        default:
            printf("输入错误，请重新输入");
        }
    }
exit:
    return;
}

int login(int cfd)
{

    int ret;
    USERMSG msg;
    system("clear");
    printf("请输入账号: ");
    fflush(stdout);
    scanf("%s", msg.id);
    printf("请输入密码: ");
    fflush(stdout);
    scanf("%s", msg.data);

    msg.type = 'L';

    USERMSG cond;
    ret = write(cfd, &msg, sizeof(msg));
    if (ret != sizeof(cond))
    {
        printf("translate error\n");
        exit(-1);
    }

    ret = read(cfd, &cond, sizeof(cond));
    if (ret != sizeof(cond))
    {
        printf("translate error\n");
        exit(-1);
    }

    printf("%c\n", cond.type);

    switch (cond.type)
    {
    case 't':
        menu(cfd, cond);
        break;
    case 'f':
        printf("账密错误\n");
        sleep(2);
        break;
    case 'n':
        printf("账号不存在\n");
        sleep(2);
        break;
    default:
        printf("translate err\n");
        sleep(2);
    }

  
    return 0;
}

int signin(int cfd)
{
    int ret;
    USERMSG msg, cond;
    system("clear");
    printf("请输入账号: ");
    fflush(stdout);
    scanf("%s", msg.id);
    printf("请输入密码: ");
    fflush(stdout);
    scanf("%s", msg.data);

    msg.type = 'S';

    
    ret = write(cfd, &msg, sizeof(msg));
    if (ret != sizeof(cond))
    {
        printf("translate error1\n");
        exit(-1);
    }
    

    ret = read(cfd, &cond, sizeof(cond));
    if (ret != sizeof(cond))
    {
        printf("translate error2\n");
        exit(-1);
    }
    

    switch (cond.type)
    {
    case 'e':
        printf("账号已存在\n");
        goto exit;
        break;
    case 't':
        break;
    default:
        printf("translate err4");
    }

exit:
    return 0;

  
}

void query()
{
}
void history()
{

}

int menu(int cfd, USERMSG msg)
{
    int i;
    while (1)
    {
        system("clear");
        printf("欢迎: %s\n", msg.name);
        printf("******请选择********\n"
               "1-------查单词-------\n"
               "2------查看历史------\n"
               "3--------退出--------\n"
               "*******************\n> ");
        scanf("%d", &i);
        switch (i)
        {
        case 1:
            query();
            break;
        case 2:
            history();
            break;
        case 3:
            goto exit;
            break;
        default:
            printf("输入有误, 请重新输入\n");
            sleep(2);
        }
    }

exit:
    return 0;
}
