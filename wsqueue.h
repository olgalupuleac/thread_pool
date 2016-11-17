#ifndef WSQUEUE_H_INCLUDED
#define WSQUEUE_H_INCLUDED
#include "squeue.h"

struct wsqueue {
	struct squeue squeue;
	pthread_cond_t cond;
};

void wsqueue_init(struct wsqueue *queue);
void wsqueue_finit(struct wsqueue *queue);

void wsqueue_push(struct wsqueue *queue, struct list_node *node);
struct list_node* wsqueue_pop(struct wsqueue *queue);

int wsqueue_wait(struct wsqueue *queue);
void wsqueue_notify(struct wsqueue *queue);
void wsqueue_notify_all(struct wsqueue *queue);


#endif // WSQUEUE_H_INCLUDED
