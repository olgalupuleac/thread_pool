#include <stdio.h>
#include <stdlib.h>
#include "thread_pool.h"

int done;
pthread_mutex_t done_mutex;
pthread_cond_t cond_exit;

struct Args {
    int left;
    int right;
    int* arr;
    struct ThreadPool* pool;
};

void pqsort(void* a);

void create_args(struct Task* task, int left, int right, int* a, struct ThreadPool* pool){
        struct Args* args = malloc(sizeof(struct Args));
        args->left = left;
        args->right = right;
        args->arr = a;
        args->pool = pool;
        task->arg = args;
        task->free_arg = 1;
}

int q_partition(int left, int right, int* a){
    int i = left; int j = right - 1;
    int ind = rand() % (right - left) + left;
    int x = a[ind];
    while(i <= j) {
        while(a[i] < x) i++;
        while(a[j] > x) j--;
        if (i <= j) {
            int tmp = a[i];
            a[i++] = a[j];
            a[j--] = tmp;
        }
    }
    return i;
}

void submit_qsort_task(int left, int right, int* a, struct ThreadPool* pool){
    if (right - left == 1) {
            pthread_mutex_lock(&done_mutex);
            done--;
            if (done == 0) pthread_cond_signal(&cond_exit);
            pthread_mutex_unlock(&done_mutex);
    }
    if (right - left > 1) {
        struct Task* task = create_task();
        task->f = pqsort;
        create_args(task, left, right, a, pool);
        thpool_submit(pool, task);
    }
}

void pqsort(void* a){
    struct Args* args = a;
    int new_border = q_partition(args->left, args->right, args->arr);
    submit_qsort_task(args->left, new_border, args->arr, args->pool);
    submit_qsort_task(new_border, args->right, args->arr, args->pool);
}

void sort_array(int* x, int N, struct ThreadPool* pool, int threads_num){
    done = N;
    pthread_mutex_init(&done_mutex, NULL);
    pthread_cond_init(&cond_exit, NULL);
    thpool_init(pool, threads_num);
    submit_qsort_task(0, N, x, pool);
    pthread_mutex_lock(&done_mutex);
    while(done > 0){
        pthread_cond_wait(&cond_exit, &done_mutex);
    }
    pthread_mutex_unlock(&done_mutex);
    thpool_finit(pool);
    pthread_mutex_destroy(&done_mutex);
    pthread_cond_destroy(&cond_exit);
    printf("Mission complete!\n");
}
