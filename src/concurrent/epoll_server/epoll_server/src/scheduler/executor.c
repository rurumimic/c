#include "../future.h"
#include "../global.h"
#include "channel.h"
#include "executor.h"
#include "poll.h"
#include "spawner.h"
#include "task.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct executor *executor_init(void)
{
	struct executor *executor = (struct executor *)malloc(sizeof(struct executor));

	if (!executor) {
		perror("executor_init: malloc failed to allocate executor");
		exit(EXIT_FAILURE);
	}

	executor->channel = channel_init();

	return executor;
}

void executor_free(struct executor *executor)
{
	if (!executor) {
		perror("executor_free: executor is NULL");
		return;
	}

	if (executor->channel) {
		channel_free(executor->channel);
	}

	free(executor);
}

struct spawner *executor_get_spawner(struct executor *executor)
{
	if (!executor) {
		perror("executor_get_spawner: executor is NULL");
		return NULL;
	}

	return spawner_init(executor->channel);
}

void executor_run(struct executor *executor)
{
	if (!executor) {
		perror("executor_run: executor is NULL");
		return;
	}

	while (running) {
		while (channel_is_empty(executor->channel)) {
			pthread_mutex_lock(&cond_mutex);
			if (pthread_cond_wait(&cond, &cond_mutex) != 0) {
				perror("executor_run: pthread_cond_wait failed");
				exit(EXIT_FAILURE);
			}
			pthread_mutex_unlock(&cond_mutex);

			if (!running) {
				printf("Close: executor\n");
				return;
			}
		}

		struct task *task = channel_recv(executor->channel);

		pthread_mutex_lock(&task->mutex);

		struct waker waker = { .ptr = task,
				       .wake = task_wake,
				       .free = task_free };
		struct context cx = from_waker(waker);

		struct poll poll = task->future->poll(task->future, cx);

		pthread_mutex_unlock(&task->mutex);

		if (poll.state == POLL_READY) {
			if (poll.free) {
				poll.free(poll.output);
			}
			task_free(task);
		}
	}
}
