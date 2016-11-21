#ifndef PQSORT_H_INCLUDED
#define PQSORT_H_INCLUDED
#include "thread_pool.h"

void sort_array(int* x, int N, struct ThreadPool* pool, int threads_num);

#endif // PQSORT_H_INCLUDED
