#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "thread_pool.h"


int x[100000];
int y[100000];
int N, threads_num;

void sort_array(int* x, int N, struct ThreadPool* pool, int threads_num);

int comp (const int *i, const int *j)
{
     return *i - *j;
}

int main()
{
  printf("Write down array size and number of threads\n");
  scanf("%d%d", &N, &threads_num);
  for (int i = 0; i < N; i++){
      x[i] = rand();
      y[i] = x[i];
  }
  qsort(y, N, sizeof (int), (int(*) (const void *, const void *)) comp);
  struct ThreadPool pool;
  sort_array(x, N, &pool, threads_num);
  for (int i = 0; i < N; i++){
      assert(x[i] == y[i]);
  }
  return 0;
}
