#ifndef PQSORT_H_INCLUDED
#define PQSORT_H_INCLUDED
#include "thread_pool.h"

struct Args {
    int left;
    int right;
    int* arr;
    struct ThreadPool* pool;
};
int q_partition(int l, int r, int* a);
void new_task(int left, int right, int* a, struct ThreadPool* pool);
void pqsort(void* a);

#endif // PQSORT_H_INCLUDED
