#include "channel.h"

#include "task.h"
#include <stdio.h>
#include <stdlib.h>

struct channel *channel_init(void) {
    struct channel *c = (struct channel *)malloc(sizeof(struct channel));

    if (!c) {
        perror("channel_init: malloc failed to allocate channel");
        exit(EXIT_FAILURE);
    }

    c->front = NULL;
    c->rear = NULL;
    c->length = 0;

    return c;
}

int channel_is_empty(struct channel *c) {
    if (!c) {
        perror("channel_is_empty: channel is NULL");
        return 1;
    }

    return c->length == 0;
}

struct task *channel_peek(struct channel *c) {
    if (!c) {
        return NULL;
    }

    if (!c->front) {
        return NULL;
    }

    return c->front->task;
}

void channel_send(struct channel *c, struct task *task) {
    if (!c) {
        perror("channel_send: channel is NULL");
        return;
    }

    struct channel_node *node =
        (struct channel_node *)malloc(sizeof(struct channel_node));

    if (!node) {
        perror("malloc failed in channel_send");
        exit(EXIT_FAILURE);
    }

    node->task = task;
    node->next = NULL;

    if (c->front) {
        c->rear->next = node;
        c->rear = node;
    } else {
        c->front = node;
        c->rear = node;
    }

    c->length++;
}

struct task *channel_recv(struct channel *c) {
    if (!c) {
        perror("channel_recv: channel is NULL");
        return NULL;
    }

    struct channel_node *node = c->front;

    if (!node) {
        return NULL;
    }

    struct task *task = node->task;
    c->front = node->next;
    c->length--;

    if (!c->front) {
        c->rear = NULL;
    }

    free(node);

    return task;
}

void channel_free(struct channel *c) {
    if (!c) {
        perror("channel_free: channel is NULL");
        return;
    }

    struct channel_node *node = c->front;

    while (node) {
        struct channel_node *next = node->next;

        if (node->task) {
          task_free(node->task);
        }

        free(node);

        node = next;
    }

    free(c);
}
