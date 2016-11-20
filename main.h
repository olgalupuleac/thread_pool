#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED
#include "thread_pool.h"


void submit_qsort_task(int left, int right, int* a, struct ThreadPool* pool);



#endif // MAIN_H_INCLUDED
