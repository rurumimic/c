#ifndef _IO_QUEUE_H
#define _IO_QUEUE_H

#include <pthread.h>
#include <stddef.h>
#include "../future.h"

enum io_ops_type {
	IO_OPS_ADD,
	IO_OPS_REMOVE,
};

struct io_ops {
	enum io_ops_type type;
	int flags;
	int fd;
	struct waker waker;
};

struct io_queue_node {
	struct io_ops *ops;
	struct io_queue_node *next;
};

struct io_queue {
	size_t length;
	struct io_queue_node *front;
	struct io_queue_node *rear;
	pthread_mutex_t mutex;
};

struct io_queue *io_queue_init(void);
void io_queue_free(struct io_queue *queue);

int io_queue_is_empty(struct io_queue *queue);
void io_queue_send(struct io_queue *queue, struct io_ops *ops);
struct io_ops *io_queue_recv(struct io_queue *queue);

#endif // _IO_QUEUE_H
