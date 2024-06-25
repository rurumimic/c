#include "../future.h"
#include "../global.h"
#include "channel.h"
#include "task.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct task *task_init(struct future *future, struct channel *channel)
{
	assert(future != NULL);
	assert(channel != NULL);

	struct task *task = (struct task *)malloc(sizeof(struct task));

	if (!task) {
		perror("task_init: malloc failed to allocate task");
		return NULL;
	}

	task->future = future;
	task->channel = channel;
	if (pthread_mutex_init(&task->mutex, NULL) != 0) {
		perror("task_init: pthread_mutex_init failed to initialize task mutex");
		free(task);
		return NULL;
	}

	return task;
}

void task_free(void *ptr)
{
	assert(ptr != NULL);

	struct task *task = (struct task *)ptr;

	task->channel = NULL;

	if (task->future) {
		task->future->free(task->future);
	}

	pthread_mutex_destroy(&task->mutex);
	free(task);
}

void task_wake(void *ptr)
{
	assert(ptr != NULL);

	struct task *task = (struct task *)ptr;

	channel_send(task->channel, task);
}
