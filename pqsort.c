#include <stdlib.h>
#include <stdio.h>
#include "pqsort.h"

int q_partition(int left, int right, int* a){
    int i = left; int j = right - 1;
    int ind = rand() % (right-left) + left;
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
    if (right - left == 1) pool->not_complete--;
    if (right - left > 1) {
        struct Task* task = malloc(sizeof(struct Task));
        task->arg = malloc(sizeof(struct Args));
        pthread_mutex_init(&task->mutex, NULL);
        //printf("Memory allocated\n");
        //fflush(stdout);
        task->f = pqsort;
        struct Args* args = malloc(sizeof(struct Args));
        args->arr = a;
        args->left = left;
        args->right = right;
        args->arr = a;
        args->pool = pool;
        task->arg = args;
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
