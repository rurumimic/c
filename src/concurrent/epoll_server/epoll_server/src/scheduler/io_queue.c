#include "io_queue.h"
#include "task.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct io_queue *io_queue_init(void)
{
	struct io_queue *queue =
		(struct io_queue *)malloc(sizeof(struct io_queue));

	if (!queue) {
		perror("io_queue_init: malloc failed to allocate io_queue");
		return NULL;
	}

	queue->front = NULL;
	queue->rear = NULL;
	queue->length = 0;

	if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
		perror("io_queue_init: pthread_mutex_init failed to initialize io_queue mutex");
		free(queue);
		return NULL;
	}

	return queue;
}

void io_queue_free(struct io_queue *queue)
{
	assert(queue != NULL);
	assert(queue->length == 0);

	pthread_mutex_destroy(&queue->mutex);
	free(queue);
}

int io_queue_is_empty(struct io_queue *queue)
{
	assert(queue != NULL);

	return queue->length == 0;
}

void io_queue_send(struct io_queue *queue, struct io_ops *ops)
{
	assert(queue != NULL);
	assert(ops != NULL);

	struct io_queue_node *node =
		(struct io_queue_node *)malloc(sizeof(struct io_queue_node));

	if (!node) {
		perror("io_queue_send: malloc failed to allocate io_queue_node");
		return;
	}

	node->ops = ops;
	node->next = NULL;

	pthread_mutex_lock(&queue->mutex);

	if (queue->front) {
		queue->rear->next = node;
		queue->rear = node;
	} else {
		queue->front = node;
		queue->rear = node;
	}

	queue->length++;

	pthread_mutex_unlock(&queue->mutex);
}

struct io_ops *io_queue_recv(struct io_queue *queue)
{
	assert(queue != NULL);

	struct io_queue_node *node = queue->front;

	if (!node) {
		return NULL;
	}

	struct io_ops *ops = node->ops;

	pthread_mutex_lock(&queue->mutex);

	queue->front = queue->front->next;
	queue->length--;

	if (!queue->front) {
		queue->rear = NULL;
	}

	pthread_mutex_unlock(&queue->mutex);

	free(node);

	return ops;
}
