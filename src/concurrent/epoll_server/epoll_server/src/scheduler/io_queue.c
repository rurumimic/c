#include "io_queue.h"
#include "task.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct io_queue *io_queue_init(void)
{
	struct io_queue *queue = (struct io_queue *)malloc(sizeof(struct io_queue));

	if (!queue) {
		perror("io_queue_init: malloc failed to allocate io_queue");
		exit(EXIT_FAILURE);
	}

	queue->front = NULL;
	queue->rear = NULL;
	queue->length = 0;

	return queue;
}

void io_queue_free(struct io_queue *queue)
{
  assert(queue != NULL);

	struct io_queue_node *node = queue->front;

	while (node) {
		struct io_queue_node *next = node->next;

		if (node->ops) {
			if (node->ops->waker.ptr) {
				node->ops->waker.free(node->ops->waker.ptr);
			}
			free(node->ops);
		}

		free(node);
		node = next;
	}

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
		exit(EXIT_FAILURE);
	}

	node->ops = ops;
	node->next = NULL;

	if (queue->front) {
		queue->rear->next = node;
		queue->rear = node;
	} else {
		queue->front = node;
		queue->rear = node;
	}

	queue->length++;
}

struct io_ops *io_queue_recv(struct io_queue *queue)
{
  assert(queue != NULL);

	struct io_queue_node *node = queue->front;

	if (!node) {
		return NULL;
	}

	struct io_ops *ops = node->ops;

	queue->front = queue->front->next;
	queue->length--;

	if (!queue->front) {
		queue->rear = NULL;
	}

	free(node);

	return ops;
}
