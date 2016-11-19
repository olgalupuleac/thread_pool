#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "main.h"

int x[100000];
int y[100000];
int N, threads_num;
extern int done;
extern pthread_mutex_t done_mutex;
extern pthread_cond_t cond_exit;
extern pthread_mutex_t guard;

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
      //printf("%d ", x[i]);
  }
  qsort(y, N, sizeof (int), (int(*) (const void *, const void *)) comp);
  //printf("\n\n");
  done = N;
  pthread_mutex_init(&done_mutex, NULL);
  pthread_mutex_init(&guard, NULL);
  pthread_cond_init(&cond_exit, NULL);
  struct ThreadPool pool;
  thpool_init(&pool, threads_num);
  submit_qsort_task(0, N, x, &pool);
  pthread_mutex_lock(&guard);
  while(done > 0){
      pthread_cond_wait(&cond_exit, &guard);
  }
  pthread_mutex_unlock(&guard);
  for (int i = 0; i < N; i++){
      //printf("%d ", x[i]);
      assert(x[i] == y[i]);
  }
  thpool_finit(&pool);
  pthread_mutex_destroy(&done_mutex);
  pthread_mutex_destroy(&guard);
  pthread_cond_destroy(&cond_exit);
  return 0;
}
