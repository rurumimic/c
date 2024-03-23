#include "task.h"
#include "future.h"
#include "channel.h"
#include "global.h"

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
	pthread_mutex_init(&t->mutex, NULL);

	return t;
}

void task_free(void *ptr)
{
	if (!ptr) {
		perror("task_free: pointer is NULL");
		return;
	}

	struct task *t = (struct task *)ptr;

  t->channel = NULL;

	if (t->future) {
		t->future->free(t->future);
	}

	pthread_mutex_destroy(&t->mutex);
	free(t);
}

void task_wake(void *ptr)
{
	if (!ptr) {
		perror("task_wake: ptr is NULL");
		return;
	}

  struct task *t = (struct task *)ptr;

	pthread_mutex_lock(&cond_mutex);
	channel_send(t->channel, t);
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&cond_mutex);
}
