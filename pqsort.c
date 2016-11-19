#include <stdlib.h>
#include <stdio.h>
#include "pqsort.h"

int done;
pthread_mutex_t done_mutex;
pthread_cond_t cond_exit;
pthread_mutex_t guard;

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
    if (i < j) return j;
    return i;
}

void new_task(int left, int right, int* a, struct ThreadPool* pool){
    if (right - left == 1) {
            pthread_mutex_lock(&done_mutex);
            done--;
            //printf("Done changed, it is %d\n", done);
            //fflush(stdout);
            if (done == 0) pthread_cond_broadcast(&cond_exit);
            pthread_mutex_unlock(&done_mutex);
    }
    if (right - left > 1) {
        struct Task* task = malloc(sizeof(struct Task));
        pthread_mutex_init(&task->mutex, NULL);
        pthread_cond_init(&task->cond, NULL);
        task->f = pqsort;
        task->free = 1;
        //printf("Memory allocated\n");
        //fflush(stdout);
        struct Args* args = malloc(sizeof(struct Args));
        args->arr = a;
        args->left = left;
        args->right = right;
        args->arr = a;
        args->pool = pool;
        task->arg = args;
        task->free_arg = 1;
        //printf("Args ready: left and right %d %d\n", args->left, args->right);
        //fflush(stdout);
        thpool_submit(pool, task);
    }
}

void pqsort(void* a){
    //printf("Hi, I'm pqsort\n");
    //fflush(stdout);
    struct Args* args = a;
    //printf("%d %d\n", args->left, args->right);
    //fflush(stdout);
    int new_border = q_partition(args->left, args->right, args->arr);
    new_task(args->left, new_border, args->arr, args->pool);
    new_task(new_border, args->right, args->arr, args->pool);
}
