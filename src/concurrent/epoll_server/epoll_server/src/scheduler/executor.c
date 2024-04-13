#include "../future.h"
#include "../global.h"
#include "channel.h"
#include "executor.h"
#include "poll.h"
#include "spawner.h"
#include "task.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct executor *executor_init(void)
{
	struct executor *executor =
		(struct executor *)malloc(sizeof(struct executor));

	if (!executor) {
		perror("executor_init: malloc failed to allocate executor");
		exit(EXIT_FAILURE);
	}

	executor->channel = channel_init();

	return executor;
}

void executor_free(struct executor *executor)
{
	assert(executor != NULL);

	if (executor->channel) {
		channel_free(executor->channel);
	}

	free(executor);
}

struct spawner *executor_get_spawner(struct executor *executor)
{
	assert(executor != NULL);

	return spawner_init(executor->channel);
}

void executor_run(struct executor *executor)
{
	assert(executor != NULL);

	struct task *task = NULL;

	while (running && (task = channel_recv(executor->channel)) != NULL) {
		struct waker waker = { .ptr = task,
				       .wake = task_wake,
				       .free = task_free };
		struct context context = from_waker(waker);

		pthread_mutex_lock(&task->mutex);

		struct poll poll = task->future->poll(task->future, context);

		pthread_mutex_unlock(&task->mutex);

		if (poll.state == POLL_READY) {
			if (poll.free) {
				poll.free(poll.output);
			}
			task_free(task);
		}
	}

	printf("Close: executor\n");
}
