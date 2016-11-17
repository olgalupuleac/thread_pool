/*void thpool_init(struct ThreadPool* pool, unsigned threads_nm); // инициализирует пул потоков, threads_nm -- число потоков
void thpool_submit(struct ThreadPool* pool, struct Task* task); // добавляет задачу на выполнение в пул потоков
void thpool_wait(struct Task* task); // возвращает управление только после того, как задача task завершилась
void thpool_finit(struct ThreadPool* pool); // финализирует пул потоков, дожидается завершения всех задач в пуле, затем освобождает ресурсы, потребляемые пулом потоков*/

#include "thread_pool.h"
#include <stdio.h>

void thpool_init(struct ThreadPool* pool, unsigned threads_nm, int not_complete){
    printf("I'm in init\n");
    fflush(stdout);
    pool->not_complete = not_complete;
    pool->threads = malloc(threads_nm * sizeof(pthread_t));
    pool->tasks = malloc(sizeof(struct wsqueue));
    pthread_cond_init(&pool->cond_exit, NULL);
    pthread_mutex_init(&pool->guard, NULL);
    printf("I'm in init\n");
    fflush(stdout);
    wsqueue_init(pool->tasks);
    pool->num = threads_nm;
    for (unsigned i = 0; i < threads_nm; i++){
        pthread_create(&pool->threads[i], NULL, thpool_go, pool);
        printf("Threads created\n");
        fflush(stdout);
    }
}

void thpool_finit(struct ThreadPool* pool){
    pool->not_complete = 0;
    for (unsigned i = 0; i < pool->num; i++){
        pthread_join(pool->threads[i], NULL);
    }
    free(pool->threads);
    wsqueue_finit(pool->tasks);
}

void thpool_submit(struct ThreadPool* pool, struct Task* task){
    wsqueue_push(pool->tasks, (struct list_node*) task);
    printf("Task pushed %d\n", queue_size(&pool->tasks->squeue.queue));
    fflush(stdout);
    printf("%p\n", task->f);
    fflush(stdout);
    printf("%p\n", task->arg);
    fflush(stdout);
}

void thpool_wait(struct Task* task){
    pthread_mutex_lock(&task->mutex);
    pthread_cond_wait(&task->cond, &task->mutex);
    pthread_mutex_unlock(&task->mutex);
}

void* thpool_go(void* arg){
    struct ThreadPool* pool = arg;
    printf("Go\n");
    fflush(stdout);
    while(pool->not_complete){
        printf("Go go\n");
        fflush(stdout);
        /*while(!queue_size(&pool->tasks->squeue.queue))*/ wsqueue_wait(pool->tasks);
        printf("I'm working %d\n", queue_size(&pool->tasks->squeue.queue));
        fflush(stdout);
        struct list_node* node = wsqueue_pop(pool->tasks);
        printf("I'm trying to get task %d\n");
        fflush(stdout);
        if (node){
            printf("getting task %d\n", pool->not_complete);
            fflush(stdout);
            struct Task* task = (struct Task*) node;
            printf("got task\n");
            fflush(stdout);
            printf("%p\n", task->f);
            fflush(stdout);
            printf("%p\n", task->arg);
            fflush(stdout);
            task->f(task->arg);
            printf("task done %d\n", pool->not_complete);
            fflush(stdout);
            free(task->arg);
            //free(task->link);
            pthread_mutex_destroy(&task->mutex);
            free(task);
            printf("clean\n");
            fflush(stdout);
        }
    }
    pthread_cond_broadcast(&pool->cond_exit);
    wsqueue_notify_all(pool->tasks);
    return NULL;
}
