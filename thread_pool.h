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
	int complete;
	int free_arg;
	int free;
};

struct ThreadPool {
    pthread_t* threads;
    struct wsqueue* tasks;
    unsigned num;
    int not_complete;
    pthread_mutex_t pool_mutex;
};

void thpool_init(struct ThreadPool* pool, unsigned threads_nm); // �������������� ��� �������, threads_nm -- ����� �������
void thpool_submit(struct ThreadPool* pool, struct Task* task); // ��������� ������ �� ���������� � ��� �������
void thpool_wait(struct Task* task); // ���������� ���������� ������ ����� ����, ��� ������ task �����������
void thpool_finit(struct ThreadPool* pool); // ������������ ��� �������, ���������� ���������� ���� ����� � ����, ����� ����������� �������, ������������ ����� �������
void* thpool_go(void* arg);
void clean(struct Task* task);

#endif // THREAD_POOL_H_INCLUDED
