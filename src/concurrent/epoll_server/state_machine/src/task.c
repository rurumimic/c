#include "task.h"
#include "hello.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct task *task_init(void)
{
	struct task *t = (struct task *)malloc(sizeof(struct task));

	if (!t) {
		perror("task_init: malloc failed to allocate task");
		exit(EXIT_FAILURE);
	}

	t->hello = hello_init();
  pthread_mutex_init(&t->mutex, NULL);

	return t;
}

void task_free(struct task *t)
{
	if (!t) {
		perror("task_free: task is NULL");
		return;
	}

	if (t->hello) {
		hello_free(t->hello);
	}

  pthread_mutex_destroy(&t->mutex);

	free(t);
}
