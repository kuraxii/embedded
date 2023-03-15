#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

pthread_mutex_t  mutex; // 定义一把互斥锁
void sys_err(char *str, int _errno)
{
  fprintf(stderr, "%s :%s\n", str, strerror(_errno));

}
void *func(void *arg)
{
  srand(time(NULL));
  while(1){
    pthread_mutex_lock(&mutex);
    printf("hello");
    sleep(1);
    printf("word\n");
    sleep(1);
    pthread_mutex_unlock(&mutex);
  }
}

int main(int argc,char *argv[])
{
  pthread_t tid;
  srand(time(NULL));
  int ret = pthread_mutex_init(&mutex, NULL);
  if(ret != 0){
    sys_err("pthread_mutex_inin err",ret);
  }

  pthread_create(&tid, NULL, func, NULL);
  while(1){
    pthread_mutex_lock(&mutex);
    printf("HELLO");
    sleep(1);
    printf("WORD\n");
    sleep(1);

    pthread_mutex_unlock(&mutex);
  }

  pthread_join(tid, NULL);

  ret = pthread_mutex_destroy(&mutex);
  if(ret != 0){
    sys_err("pthread_mutex_inin err",ret);
  }
  pthread_exit(NULL);
}