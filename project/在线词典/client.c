#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>
char buf[1024] = {0};
typedef struct
{
    char type;
    char id[20];
    char name[20];
    char data[128];
} USERMSG;

USERMSG curr;

int _connect();
int menu(int cfd, USERMSG msg);
void loginmenu(int cfd);
int signin(int cfd);
int login(int cfd);
void query(int cfd); // 查询
void history(int cfd);
void Write(int cfd, USERMSG msg);

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
        printf("*******请选择*********\n"
               "*1------登录---------*\n"
               "*2------注册---------*\n"
               "*3------退出---------*\n"
               "**********************\n> ");
        scanf("%d", &i);
        switch (i)
        {
        case 1:
            login(cfd);
            break;
        case 2:
            if (signin(cfd) == 0)
            {
                continue;
            }
            break;
        case 3:
            goto exit;
            break;
        default:
            printf("输入错误，请重新输入\n");
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

    

    switch (cond.type)
    {
    case 't':
        strcpy(curr.id, msg.id);
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
    printf("请输入昵称: ");
    fflush(stdout);
    scanf("%s", msg.name);
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
        sleep(1);
        return 1;
        break;
    case 't':
        printf("注册成功!\n");
        sleep(1);
        return 0;
        break;
    default:
        printf("translate err4");
        return -1;
    }
}

void query(int cfd)
{
    int ret;
    USERMSG msg, buf;
    msg.type = 'Q';
    while (1)
    {
        printf("<word> ");
        fflush(stdout);
        scanf("%s", msg.data);

        if (strncmp(msg.data, ".quit", 5) == 0)
        {
            msg.type = 'x';
            Write(cfd, msg);
            break;
        }

        Write(cfd, msg);

        ret = read(cfd, &buf, sizeof(buf));
        if (ret == -1)
        {
            perror("read");
            exit(-1);
        }

        if (ret == 0)
        {
            printf("server is close\n");
            exit(-1);
        }

        if (ret != sizeof(USERMSG))
        {
            printf("translate error\n");
            exit(-1);
        }

        printf("<translate> %s\n", buf.data);
    }
}
void history(int cfd)
{
    int ret;
    USERMSG msg, buf;
    msg.type = 'H';
    Write(cfd, msg);
    while (1)
    {

        ret = read(cfd, &buf, sizeof(buf));
        if (ret == -1)
        {
            perror("read");
            exit(-1);
        }
        if (ret == 0)
        {
            exit(-1);
        }
        if (ret != sizeof(USERMSG))
        {
            printf("translate error\n");
            exit(-1);
        }
        if (buf.type == 'o')
            break;
        if (buf.type == 'h')
            printf("%s\n", buf.data);
    }
}

int menu(int cfd, USERMSG msg)
{
    int i;
    USERMSG req;
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
            req.type = 'Q';
            Write(cfd, req);
         
            query(cfd);
            break;
        case 2:
            history(cfd);
            struct termios old_termios, new_termios;

            // 获取终端的原始设置
            tcgetattr(STDIN_FILENO, &old_termios);
            new_termios = old_termios;

            // 禁用行缓冲和回显
            new_termios.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

            printf("按下任意键后返回...\n");

            // 从终端读取一个字符
            char ch;
            read(STDIN_FILENO, &ch, 1);

            // 恢复终端的原始设置
            tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);

            break;
        case 3:
            req.type = 'O';
            Write(cfd, req);
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

void Write(int cfd, USERMSG msg)
{
    int ret;
    ret = write(cfd, &msg, sizeof(msg));
    if (ret != sizeof(msg))
    {
        printf("translate error\n");
        exit(-1);
    }
}