#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <semaphore.h>
void sys_err(char *str, int _errno)
{
  fprintf(stderr, "%s :%s\n", str, strerror(_errno));
}

void* consumer(void* arg);   //消费者
void* product(void* arg);   //生产者

#define NUM 5
int sem_quene[NUM];
sem_t blank_number, product_number;   //队列空白信号量数量  产品信号量数量



int main(int argc,char *argv[])
{
  srand(time(NULL));  //随机数种子
  pthread_t cid, pid;
  sem_init(&blank_number, 0, 5);
  sem_init(&product_number, 0, 0);  // 信号量队列初始化

  pthread_create(&pid, NULL, product, NULL);
  pthread_create(&cid, NULL, consumer, NULL);

  pthread_join(pid, NULL);   //线程回收
  pthread_join(cid, NULL);

  sem_destroy(&blank_number);   //信号量回收
  sem_destroy(&product_number);


  pthread_exit(NULL);
}



void* product(void* arg)   //生产者
{   
  int i = 0;

  while(1){
    sem_wait(&blank_number);

    sem_quene[i] = rand() % 1000;   //临界区
    printf("----product %d\n",sem_quene[i]);


    sem_post(&product_number);    //将产品数++
    i = (i + 1) % NUM;    //循环队列
    sleep(1);
    
   
    
    sleep(rand() % 1);
  }

  return NULL;
}

void* consumer(void* arg)   //消费者
{  
  int i = 0;

  while(1){
    sem_wait(&product_number);



    printf("-Consume--- %d\n",sem_quene[i]);  //临界区
    sem_quene[i] = 0;

    sem_post(&blank_number);   //空格数++
    
    i = (i+1) % NUM;


    
    
    sleep(rand() % 3);

  }

  return NULL;
}