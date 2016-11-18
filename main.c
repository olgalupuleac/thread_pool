#include <stdio.h>
#include <stdlib.h>
#include "pqsort.h"

int x[100000];
int N;

int main()
{
  N = 10;
  for (int i=0; i<N; i++){
    x[i] = rand();
    printf("%d ", x[i]);
  }
  printf("\n");
  struct ThreadPool pool;
  thpool_init(&pool, 2, N);
  new_task(0, N, x, &pool);
  while(pool.not_complete > 0){
    pthread_mutex_lock(&pool.guard);
    pthread_cond_wait(&pool.cond_exit, &pool.guard);
    pthread_mutex_unlock(&pool.guard);
  }
  for (int i=0; i<N; i++){
    printf("%d ", x[i]);
  }
  thpool_finit(&pool);
  return 0;
}
