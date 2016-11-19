/*void thpool_init(struct ThreadPool* pool, unsigned threads_nm); // инициализирует пул потоков, threads_nm -- число потоков
void thpool_submit(struct ThreadPool* pool, struct Task* task); // добавляет задачу на выполнение в пул потоков
void thpool_wait(struct Task* task); // возвращает управление только после того, как задача task завершилась
void thpool_finit(struct ThreadPool* pool); // финализирует пул потоков, дожидается завершения всех задач в пуле, затем освобождает ресурсы, потребляемые пулом потоков*/

#include "thread_pool.h"
#include <stdio.h>

void thpool_init(struct ThreadPool* pool, unsigned threads_nm){
    //printf("I'm in init\n");
    //fflush(stdout);
    pool->not_complete = 1;
    pthread_mutex_init(&pool->pool_mutex, NULL);
    pool->threads = malloc(threads_nm * sizeof(pthread_t));
    pool->tasks = malloc(sizeof(struct wsqueue));
    //printf("I'm in init\n");
    //fflush(stdout);
    wsqueue_init(pool->tasks);
    pool->num = threads_nm;
    for (unsigned i = 0; i < threads_nm; i++){
        pthread_create(&pool->threads[i], NULL, thpool_go, pool);
        //printf("Threads created\n");
        //fflush(stdout);
    }
}

void thpool_finit(struct ThreadPool* pool){
    //printf("Finished\n");
    //fflush(stdout);
    pthread_mutex_lock(&pool->pool_mutex);
    pool->not_complete = 0;
    //printf("not complete changed it's now %d\n",  pool->not_complete);
    //fflush(stdout);
    pthread_mutex_unlock(&pool->pool_mutex);
    for (unsigned i = 0; i < pool->num; i++){
        struct Task* task = create_task();
        task->arg = NULL;
        task->f = NULL;
        thpool_submit(pool, task);
    }
    for (unsigned i = 0; i < pool->num; i++){
        pthread_join(pool->threads[i], NULL);
    }
    while(queue_size(&pool->tasks->squeue.queue)){
        struct list_node* node = wsqueue_pop(pool->tasks);
        struct Task* task = (struct Task*) node;
        destroy_task(task);
    }
    pthread_mutex_destroy(&pool->pool_mutex);
    free(pool->threads);
    wsqueue_finit(pool->tasks);
}

void thpool_submit(struct ThreadPool* pool, struct Task* task){
    wsqueue_push(pool->tasks, (struct list_node*) task);
    //printf("Task pushed %d\n", queue_size(&pool->tasks->squeue.queue));
    //fflush(stdout);
    /*printf("%p\n", task->f);
    fflush(stdout);
    printf("%p\n", task->arg);
    fflush(stdout);*/
}

void thpool_wait(struct Task* task){
    pthread_mutex_lock(&task->mutex);
    while(! task->complete){
        pthread_cond_wait(&task->cond, &task->mutex);
    }
    pthread_mutex_unlock(&task->mutex);
}

void* thpool_go(void* arg){
    struct ThreadPool* pool = arg;
    //printf("Go\n");
    //fflush(stdout);
    pthread_mutex_lock(&pool->pool_mutex);
    //printf("Lock\n");
    //fflush(stdout);
    while(pool->not_complete){
        pthread_mutex_unlock(&pool->pool_mutex);
        //printf("Unlock in while\n");
        //fflush(stdout);
        wsqueue_wait(pool->tasks);
        //printf("I'm working %d\n", queue_size(&pool->tasks->squeue.queue));
        //fflush(stdout);
        //pthread_mutex_lock(&pool->pool_mutex);
        struct list_node* node = wsqueue_pop(pool->tasks);
        //pthread_mutex_unlock(&pool->pool_mutex);
        //printf("I'm trying to get task %d\n");
        //fflush(stdout);
        if(node){
            //printf("getting task %d\n", pool->not_complete);
            //fflush(stdout);
            struct Task* task = (struct Task*) node;
            /*printf("got task\n");
            fflush(stdout);
            printf("%p\n", task->f);
            fflush(stdout);
            printf("%p\n", task->arg);
            fflush(stdout);*/
            if(task->f) task->f(task->arg);
            destroy_task(task);
        }
        pthread_mutex_lock(&pool->pool_mutex);
        //printf("Lock in while\n");
        //fflush(stdout);
    }
    pthread_mutex_unlock(&pool->pool_mutex);
    //printf("Unlock\n");
    //fflush(stdout);
    //printf("Goodbye\n");
    //fflush(stdout);
    return NULL;
}
struct Task* create_task(void){
    struct Task* task = malloc(sizeof(struct Task));
    task->free_arg = 0;
    task->free = 1;
    pthread_mutex_init(&task->mutex, NULL);
    pthread_cond_init(&task->cond, NULL);
    return task;
};

void destroy_task(struct Task* task){
    pthread_mutex_destroy(&task->mutex);
    pthread_cond_destroy(&task->cond);
    if (task->free_arg) free(task->arg);
    if (task->free) free(task);
}
