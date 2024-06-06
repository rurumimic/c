#include "../future.h"
#include "../global.h"
#include "channel.h"
#include "executor.h"
#include "../poll.h"
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
		return NULL;
	}

	struct channel *channel = channel_init();
	if (!channel) {
		free(executor);
		return NULL;
	}

	executor->channel = channel;
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

int executor_spawn(struct executor *executor, pthread_t *__restrict __newthread)
{
	assert(executor != NULL);

	if (pthread_create(__newthread, NULL, executor_run, (void *)executor) !=
	    0) {
		perror("executor_spawn: pthread_create failed to create executor thread");
		return -1;
	}

	return 0;
}

void executor_cancel(void *arg)
{
	printf("Executor Stop.\n");
}

void *executor_run(void *arg)
{
	assert(arg != NULL);

	pthread_cleanup_push(executor_cancel, NULL);

	struct executor *executor = (struct executor *)arg;

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

	pthread_cleanup_pop(0);

	return NULL;
}
