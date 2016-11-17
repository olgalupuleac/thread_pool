#ifndef SQUEUE_H_INCLUDED
#define SQUEUE_H_INCLUDED

#include <pthread.h>
#include "queue.h"

struct squeue {
	struct queue queue;
	pthread_mutex_t mutex;
};

void squeue_init(struct squeue *squeue);
void squeue_finit(struct squeue *squeue);

void squeue_push(struct squeue *squeue, struct list_node *node);
struct list_node *squeue_pop(struct squeue *squeue);

#endif // SQUEUE_H_INCLUDED
