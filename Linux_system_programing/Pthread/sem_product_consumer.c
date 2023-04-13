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

void *consumer(void *arg); // 消费者
void *product(void *arg);  // 生产者

#define NUM 10
int sem_quene[NUM];

// 生产者 消费者互斥量
pthread_mutex_t pro_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t con_mutex = PTHREAD_MUTEX_INITIALIZER;

int product_i = 0;
int consumer_i = 0;

sem_t blank_number, product_number; // 队列空白信号量数量  产品信号量数量

int main(int argc, char *argv[])
{
    srand(time(NULL)); // 随机数种子
    pthread_t cid, pid;
    sem_init(&blank_number, 0, 5);
    sem_init(&product_number, 0, 0); // 信号量队列初始化
    

    pthread_create(&pid, NULL, product, (void *)(intptr_t)1); // 创建生产者与消费者
    pthread_create(&pid, NULL, product, (void *)2);   // 生产者2
    pthread_create(&cid, NULL, consumer, (void *)(intptr_t)1); // 消费者1
    pthread_create(&cid, NULL, consumer, (void *)2);  //消费者2

    pthread_join(pid, NULL); // 线程回收
    pthread_join(cid, NULL);

    sem_destroy(&blank_number); // 信号量回收
    sem_destroy(&product_number);

    pthread_exit(NULL);
}

void *product(void *arg) // 生产者
{
    

    while (1)
    {
        pthread_mutex_lock(&pro_mutex);
        sem_wait(&blank_number);
        sem_quene[product_i] = rand() % 1000; // 临界区
        printf("product%d: put %d to buffer%d\n", (int)(intptr_t)arg, sem_quene[product_i], product_i);
        sem_post(&product_number); // 将产品数++
        product_i = (product_i + 1) % NUM;         // 循环队列
        pthread_mutex_unlock(&pro_mutex);
        sleep(1);

        sleep(rand() % 1);
    }

    return NULL;
}

void *consumer(void *arg) // 消费者
{
    

    while (1)
    {
        pthread_mutex_lock(&con_mutex);
        sem_wait(&product_number);

        printf("consumer%d: get %d to buffer%d\n", (int)(intptr_t)arg, sem_quene[consumer_i], consumer_i); // 临界区
        sem_quene[consumer_i] = 0;

        sem_post(&blank_number); // 空格数++
        consumer_i = (consumer_i + 1) % NUM;
        pthread_mutex_unlock(&con_mutex);

        sleep(rand() % 3);
    }

    return NULL;
}