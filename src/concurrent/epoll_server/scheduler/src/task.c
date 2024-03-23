#include "task.h"
#include "hello.h"
#include "channel.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct task *task_init(struct hello *h, struct channel *c)
{
	if (!h) {
		perror("task_init: hello is NULL");
		return NULL;
	}

	if (!c) {
		perror("task_init: channel is NULL");
		return NULL;
	}

	struct task *t = (struct task *)malloc(sizeof(struct task));

	if (!t) {
		perror("task_init: malloc failed to allocate task");
		exit(EXIT_FAILURE);
	}

	t->hello = h;
	t->channel = c;
	pthread_mutex_init(&t->mutex, NULL);

	return t;
}

void task_wake_by_ref(struct task *t)
{
	if (!t) {
		perror("task_wake_by_ref: task is NULL");
		return;
	}

	channel_send(t->channel, sizeof(struct task), t, task_free);
}

void task_free(void *p)
{
	if (!p) {
		perror("task_free: task is NULL");
		return;
	}

	struct task *t = (struct task *)p;

	if (t->hello) {
		hello_free(t->hello);
	}

	pthread_mutex_destroy(&t->mutex);

	free(t);
}
