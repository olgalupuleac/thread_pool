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

void thpool_init(struct ThreadPool* pool, unsigned threads_nm, int not_complete); // �������������� ��� �������, threads_nm -- ����� �������
void thpool_submit(struct ThreadPool* pool, struct Task* task); // ��������� ������ �� ���������� � ��� �������
void thpool_wait(struct Task* task); // ���������� ���������� ������ ����� ����, ��� ������ task �����������
void thpool_finit(struct ThreadPool* pool); // ������������ ��� �������, ���������� ���������� ���� ����� � ����, ����� ����������� �������, ������������ ����� �������
void* thpool_go(void* arg); // ������ �������


#endif // THREAD_POOL_H_INCLUDED
