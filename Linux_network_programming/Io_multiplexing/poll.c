#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
void sys_err(char *str)
{
  perror(str);
  exit(1);
}

int main(int argc, char *argv[])
{
  int i, ret, maxfd = 0, lfd, client_fd, lastfd, nread;
  char buf[BUFSIZ];
  struct sockaddr_in ser_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  struct pollfd fds[FD_SETSIZE];
  for (i = 0; i < FD_SETSIZE; i++)
  {
    fds[i].fd = -1;
  }

  lfd = socket(AF_INET, SOCK_STREAM, 0);
  if (lfd == -1)
  {
    sys_err("socket err");
  }
  int opt = 1;
  setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  ser_addr.sin_family = AF_INET;
  ser_addr.sin_port = htons(12500);
  ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  ret = bind(lfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
  if (ret == -1)
  {
    sys_err("bind err");
  }

  listen(lfd, 255);

  fds[0].fd = lfd;
  fds[0].events = POLLIN;
  fds[0].revents = 0;

  lastfd = 0;

  while (1)
  {

    // printf("before poll----\n");
    // for(i = 0; i <= maxfd; i++){
    //     if(fds[i].fd != -1){
    //       printf("i = %d,fd = %d,return = %d\n", i, fds[i].fd, fds[i].revents&POLLIN);
    //     }

    //   }
    nread = poll(fds, lastfd + 1, -1);
    // printf("after poll----\n");
    if (nread < 0)
    {
      sys_err("poll err");
    }

    if (fds[0].revents & POLLIN)
    { // 建立通信
      //  printf("before accept----\n");
      client_fd = accept(lfd, (struct sockaddr *)&client_addr, &client_addr_len);
      //  printf("after accept----\n");
      if (client_fd == -1)
      {
        sys_err("accept err");
      }
      printf("client connect success: ip = %s, port = %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
      for (i = 1; i < FD_SETSIZE; i++)
      { // 添加监 文件描述符
        if (fds[i].fd == -1)
        {
          fds[i].fd = client_fd;
          fds[i].events = POLLIN;
          fds[i].revents = 0;
          maxfd++;
          break;
        }
      }

      if (lastfd < i)
      {
        lastfd = i;
      }

      if (--nread == 0)
      {
        continue;
      }
    }

    for (i = 1; i <= maxfd; i++)
    {
      if (fds[i].revents & POLLIN)
      {
        ret = read(fds[i].fd, buf, sizeof(buf));
        if (ret == -1)
        {
          sys_err("read err");
        }
        if (ret == 0)
        {
          close(fds[i].fd);
          fds[i].fd = -1; /* poll中不监控该文件描述符,直接置为-1即可,不用像select中那样移除*/
        }
        if (ret > 0)
        {
          buf[ret] = '\0';
          printf("from client：%s\n", buf);
        }
      }
    }
  }

  return 0;
}