/*********************************************
 * @FileName: myshell.c
 * @Author: Null-zzj
 * @Mail: zj.zhu.cn@gmail.com
 * @CreatedTime: Mon Jul 03 13:17:29 2023
 * @Descript:
 */

#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/wait.h>
#include <unistd.h>

char *username;
char hostname[255] = {0};
char cwd[255] = {0};
char cmd[255] = {0};
char *argv[20] = {0};

int count;

void hander(int sig)
{
    puts("");
    printf("\033[;32m%s@%s\033[0m \033[;34m%s\033[0m $ ", username, hostname, cwd);
}

void strucArgv(char *buf);
int main()
{
    signal(SIGINT, hander);
    pid_t pid;
    struct passwd *pwd;
    pwd = getpwuid(getuid());
    username = pwd->pw_name;

    int ret = gethostname(hostname, sizeof(hostname) - 1);

    getcwd(cwd, sizeof(cwd) - 1);

    while (1)
    {
        getcwd(cwd, sizeof(cwd) - 1);
        printf("\033[;32m%s@%s\033[0m \033[;34m%s\033[0m $ ", username, hostname, cwd);
        fflush(stdin);
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strlen(cmd) - 1] = '\0';
        strucArgv(cmd);
        if (*cmd == '\n')
        {
            continue;
        }

        if (!strcmp("cd", argv[0])) // cd命令
        {
            if (chdir(argv[1]) == -1)
            {
                perror("cd");
            }
            goto over;
        }
        if (!strcmp("exit", argv[0])) // exit命令
        {
            break;
        }

        if ((pid = fork()) < 0)
        {
            perror("fork");
        }
        else if (pid == 0)
        {
            if (execvp(argv[0], argv) == -1)
            {
                perror(argv[0]);
            }
        }
        else
        {
            wait(NULL);
        }
    over:
        bzero(cmd, sizeof(cmd));
        for (int i = 0; i < count; i++)
        {
            argv[i] = NULL;
        }
        count = 0;
    }

    return 0;
}

void strucArgv(char *buf)
{

    argv[count++] = strtok(buf, " ");
    while ((argv[count] = strtok(NULL, " ")) != NULL)
    {
        count++;
    }
}