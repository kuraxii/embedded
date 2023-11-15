#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
void sys_err(char *str) {
  perror(str);
  exit(1);
}

int main(int argc, char *argv[]) {
  int cfd;
  int ret;
  char buf[1024];
  cfd = socket(AF_INET, SOCK_STREAM, 0);
  if (cfd == -1) {
    sys_err("scoket client err");
  }
  struct sockaddr_in ser_addr;
  ser_addr.sin_family = AF_INET;
  ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  ser_addr.sin_port = htons(12500);
  ret = connect(cfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
  if (ret == -1) {
    sys_err("connect err");
  }
  while (1) {
    scanf("%s", buf);

    write(cfd, buf, sizeof(buf));
  }

  close(cfd);

  return 0;
}