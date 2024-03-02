#include "task.h"
#include "future.h"
#include "channel.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct task *task_init(struct future *f, struct channel *c)
{
	if (!f) {
		perror("task_init: future is NULL");
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

	t->future = f;
	t->channel = c;
	pthread_mutex_init(&t->future_mutex, NULL);

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

	if (t->future) {
    // TODO: future free
		free(t->future);
	}

	pthread_mutex_destroy(&t->future_mutex);

	free(t);
}
