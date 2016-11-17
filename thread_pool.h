#ifndef THREAD_POOL_H_INCLUDED
#define THREAD_POOL_H_INCLUDED
#include <stdlib.h>
#include "wsqueue.h"

struct Task {
    struct list_node link;
	void (*f)(void *);
	void* arg;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	//bool not_complete;
};

struct ThreadPool {
    pthread_t* threads;
    struct wsqueue* tasks;
    unsigned num;
    int not_complete;
    pthread_cond_t cond_exit;
	pthread_mutex_t guard;
};

void thpool_init(struct ThreadPool* pool, unsigned threads_nm, int not_complete); // инициализирует пул потоков, threads_nm -- число потоков
void thpool_submit(struct ThreadPool* pool, struct Task* task); // добавляет задачу на выполнение в пул потоков
void thpool_wait(struct Task* task); // возвращает управление только после того, как задача task завершилась
void thpool_finit(struct ThreadPool* pool); // финализирует пул потоков, дожидается завершения всех задач в пуле, затем освобождает ресурсы, потребляемые пулом потоков
void* thpool_go(void* arg); // работа потоков


#endif // THREAD_POOL_H_INCLUDED
