#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
int main(int argc,char *argv[])
{
  DIR* dp;
  struct dirent *sdp;
  dp = opendir(argv[1]);
  if(dp == NULL){
    perror("opendir err");
    exit(1);
  }
  while ((sdp = readdir(dp)) != NULL)
  {
    printf("%s\t", sdp->d_name);
  }
  printf("\n");

  closedir(dp);



  return 0;
}