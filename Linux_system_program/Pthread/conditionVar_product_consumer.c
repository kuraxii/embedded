#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
void sys_err(char *str, int _errno)
{
  fprintf(stderr, "%s :%s\n", str, strerror(_errno));
}

struct msg{
  int num;
  struct msg* next;
};
struct msg* head;

void* product(void* arg);
void* consumer(void* arg);


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int main(int argc,char *argv[])
{
  int ret;
  pthread_t pid,cid;
  srand(time(NULL));
  ret = pthread_create(&pid, NULL, product, NULL);  //生产者
  if(ret != 0){
    sys_err("pthread_create err", ret);
  }
  pthread_create(&cid, NULL, consumer, NULL);    //消费者
  if(ret != 0){
    sys_err("pthread_create err", ret);
  }

  pthread_create(&cid, NULL, consumer, NULL);    //消费者
  if(ret != 0){
    sys_err("pthread_create err", ret);
  }

  pthread_create(&cid, NULL, consumer, NULL);    //消费者
  if(ret != 0){
    sys_err("pthread_create err", ret);
  }

  pthread_join(pid, NULL);
  if(ret != 0){
    sys_err("pthread_join err", ret);
  }
  pthread_join(cid, NULL);
  if(ret != 0){
    sys_err("pthread_join err", ret);
  }


  return 0;
}

void* product(void* arg){   //生产者

  while(1){
    
    struct msg *mp = (struct msg*)malloc(sizeof(struct msg));
    mp->num = rand() % 1000;         //模拟生产数据
    pthread_mutex_lock(&mutex);
    mp->next = head;                 //写公共区域
    head = mp;
    
    pthread_mutex_unlock(&mutex);
    printf("product %d------\n", mp->num);
    pthread_cond_signal(&cond);    //唤醒阻塞在条件变量的线程 生产者唤醒消费者
    
    sleep(rand() % 3);
  }

  return NULL;
}

void* consumer(void* arg){  //消费者
  while(1){
    pthread_mutex_lock(&mutex);
    while(head == NULL){         // 检查条件变量
      pthread_cond_wait(&cond, &mutex);   //阻塞等待条件边量   阻塞过程中解锁，  阻塞结束后加锁
    }
     struct msg *mp = head;
     head = mp->next;

    pthread_mutex_unlock(&mutex);
    printf("------consumer id = %lu : %d\n",pthread_self(), mp->num);
    free(mp);
    sleep(rand() % 3);

  }

  return NULL;
}