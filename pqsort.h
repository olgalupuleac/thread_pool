#ifndef PQSORT_H_INCLUDED
#define PQSORT_H_INCLUDED
#include "thread_pool.h"

struct Args {
    int left;
    int right;
    int* arr;
    struct ThreadPool* pool;
};

//void submit_qsort_task(int left, int right, int* a, struct ThreadPool* pool);
void pqsort(void* a);
int q_partition(int left, int right, int* a);
void create_args(struct Task* task, int left, int right, int* a, struct ThreadPool* pool);

#endif // PQSORT_H_INCLUDED
