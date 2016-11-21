/*void thpool_init(struct ThreadPool* pool, unsigned threads_nm); // инициализирует пул потоков, threads_nm -- число потоков
void thpool_submit(struct ThreadPool* pool, struct Task* task); // добавляет задачу на выполнение в пул потоков
void thpool_wait(struct Task* task); // возвращает управление только после того, как задача task завершилась
void thpool_finit(struct ThreadPool* pool); // финализирует пул потоков, дожидается завершения всех задач в пуле, затем освобождает ресурсы, потребляемые пулом потоков*/

#include "thread_pool.h"

void* thpool_go(void* arg){
    struct ThreadPool* pool = arg;
    while(1){
        wsqueue_wait(pool->tasks);
        struct list_node* node = wsqueue_pop(pool->tasks);
        if(node){
            struct Task* task = (struct Task*) node;
            wsqueue_push(pool->tasks_to_delete, (struct list_node*) task);
            if(task->f) task->f(task->arg);
            pthread_mutex_lock(&task->mutex);
            task->complete = 1;
            pthread_cond_signal(&task->cond);
            pthread_mutex_unlock(&task->mutex);
        }
    }
    return NULL;
}

struct Task* create_task(void){
    struct Task* task = malloc(sizeof(struct Task));
    task->free_arg = 0;
    task->free = 1;
    task->complete = 0;
    pthread_mutex_init(&task->mutex, NULL);
    pthread_cond_init(&task->cond, NULL);
    return task;
}

void destroy_task(struct Task* task){
    pthread_mutex_destroy(&task->mutex);
    pthread_cond_destroy(&task->cond);
    if (task->free_arg) free(task->arg);
    if (task->free) free(task);
}

void thpool_init(struct ThreadPool* pool, unsigned threads_nm){
    pool->is_working = 1;
    pthread_mutex_init(&pool->pool_mutex, NULL);
    pool->threads = malloc(threads_nm * sizeof(pthread_t));
    pool->tasks = malloc(sizeof(struct wsqueue));
    pool->tasks_to_delete = malloc(sizeof(struct wsqueue));
    wsqueue_init(pool->tasks);
    wsqueue_init(pool->tasks_to_delete);
    pool->num = threads_nm;
    for (unsigned i = 0; i < threads_nm; i++){
        pthread_create(&pool->threads[i], NULL, thpool_go, pool);
    }
}

void thpool_finit(struct ThreadPool* pool){
    pthread_mutex_lock(&pool->pool_mutex);
    pool->is_working = 0;
    pthread_mutex_unlock(&pool->pool_mutex);
    for (unsigned i = 0; i < pool->num; i++){
        struct Task* task = create_task();
        task->arg = NULL;
        task->f = pthread_exit;
        wsqueue_push(pool->tasks, (struct list_node*) task);
    }
    for (unsigned i = 0; i < pool->num; i++){
        pthread_join(pool->threads[i], NULL);
    }
    while(queue_size(&pool->tasks_to_delete->squeue.queue)){
        struct list_node* node = wsqueue_pop(pool->tasks_to_delete);
        struct Task* task = (struct Task*) node;
        destroy_task(task);
    }
    pthread_mutex_destroy(&pool->pool_mutex);
    free(pool->threads);
    wsqueue_finit(pool->tasks);
    wsqueue_finit(pool->tasks_to_delete);
    free(pool->tasks);
    free(pool->tasks_to_delete);
}

void thpool_submit(struct ThreadPool* pool, struct Task* task){
    pthread_mutex_lock(&pool->pool_mutex);
    if(pool->is_working) wsqueue_push(pool->tasks, (struct list_node*) task);
    pthread_mutex_unlock(&pool->pool_mutex);
}

void thpool_wait(struct Task* task){
    pthread_mutex_lock(&task->mutex);
    while(!task->complete){
        pthread_cond_wait(&task->cond, &task->mutex);
    }
    pthread_mutex_unlock(&task->mutex);
}

