#include "task.h"
#include "future.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct task *task_init(struct future *future)
{
	struct task *t = (struct task *)malloc(sizeof(struct task));

	if (!t) {
		perror("task_init: malloc failed to allocate task");
		exit(EXIT_FAILURE);
	}

	t->future = future;
	pthread_mutex_init(&t->mutex, NULL);

	return t;
}

void task_free(void *ptr)
{
	if (!ptr) {
		perror("task_free: ptr is NULL");
		return;
	}

	struct task *t = (struct task *)ptr;

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

	// nothing to do here
}
