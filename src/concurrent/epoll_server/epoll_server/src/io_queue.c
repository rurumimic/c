#include "io_queue.h"
#include "task.h"

#include <stdio.h>
#include <stdlib.h>

struct io_queue *io_queue_init(void) {
    struct io_queue *q = (struct io_queue *)malloc(sizeof(struct io_queue));

    if (!q) {
        perror("io_queue_init: malloc failed to allocate io_queue");
        exit(EXIT_FAILURE);
    }

    q->front = NULL;
    q->rear = NULL;
    q->length = 0;

    return q;
}

int io_queue_is_empty(struct io_queue *q) {
    if (!q) {
        perror("io_queue_is_empty: io_queue is NULL");
        return 1;
    }

    return q->length == 0;
}

void io_queue_send(struct io_queue *q, struct io_ops *ops) {
    if (!q) {
        perror("io_queue_send: io_queue is NULL");
        return;
    }

    struct io_queue_node *node = (struct io_queue_node *)malloc(sizeof(struct io_queue_node));

    if (!node) {
        perror("io_queue_send: malloc failed to allocate io_queue_node");
        exit(EXIT_FAILURE);
    }

    node->ops = ops;
    node->next = NULL;

    if (q->front) {
        q->rear->next = node;
        q->rear = node;
    } else {
        q->front = node;
        q->rear = node;
    }

    q->length++;
}

struct io_ops *io_queue_recv(struct io_queue *q) {
    if (!q) {
        perror("io_queue_recv: io_queue is NULL");
        return NULL;
    }

    struct io_queue_node *node = q->front;

    if (!node) {
        return NULL;
    }

    struct io_ops *ops = node->ops;

    q->front = q->front->next;
    q->length--;

    if (!q->front) {
        q->rear = NULL;
    }

    free(node);

    return ops;
}

void io_queue_free(struct io_queue *q) {
    if (!q) {
        perror("io_queue_free: io_queue is NULL");
        return;
    }

    struct io_queue_node *node = q->front;

    while (node) {
        struct io_queue_node *next = node->next;

        if (node->ops) {
          if (node->ops->task) {
            task_free(node->ops->task);
          }
          free(node->ops);
        }

        free(node);
        node = next;
    }

    free(q);
}

