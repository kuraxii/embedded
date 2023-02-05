#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

void isfile(char *name);
void read_dir(char *dir);

void read_dir(char *dir)
{
  char path[256];
  DIR *dp;
  struct dirent *sdp;
  // 打开目录
  dp = opendir(dir);
  if (dp == NULL)
  {
    perror("opendir err");
    return;
  }

  // 读取目录项
  while ((sdp = readdir(dp)) != NULL)
  {
    // 去除掉 . .. 目录，以防止无线递归
    if (!strcmp(sdp->d_name, ".") | !strcmp(sdp->d_name, ".."))
    {
      continue;
    }
    // 目录项本身不可访问，故需要拼接
    if (strlen(dir)+strlen(sdp->d_name)+2 > 256)
    {
      perror("目录名错误");
      return;
    }
    
    sprintf(path, "%s/%s", dir, sdp->d_name);

    // 判断目录项是否是目录
    isfile(path);
  }

  closedir(dp);

  return;
}

void isfile(char *name)
{
  int ret;
  struct stat sbuf;
  // 获取文件路径信息
  ret = stat(name, &sbuf);
  if (ret == -1)
  {
    perror("stat err");
    return;
  }
  // 判断是否是文件
  if (S_ISDIR(sbuf.st_mode))
  {
    read_dir(name);
  }

  // 打印出文件名
  printf("%-50s\t\t%ld\n", name, sbuf.st_size);

  return;
}

int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    isfile(".");
  }
  else
  {
    isfile(argv[1]);
  }
  return 0;
}