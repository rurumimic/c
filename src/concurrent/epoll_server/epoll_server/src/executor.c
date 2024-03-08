#include "executor.h"
#include "channel.h"
#include "future.h"
#include "spawner.h"
#include "task.h"
#include "global.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct executor *executor_init(void)
{
	struct executor *e = (struct executor *)malloc(sizeof(struct executor));

	if (!e) {
		perror("executor_init: malloc failed to allocate executor");
		exit(EXIT_FAILURE);
	}

	e->channel = channel_init();

	return e;
}

void executor_free(struct executor *e)
{
	if (!e) {
		perror("executor_free: executor is NULL");
		return;
	}

	if (e->channel) {
		channel_free(e->channel);
	}

	free(e);
}

struct spawner *executor_get_spawner(struct executor *e)
{
	if (!e) {
		perror("executor_get_spawner: executor is NULL");
		return NULL;
	}

	return spawner_init(e->channel);
}

void executor_run(struct executor *e)
{
	if (!e) {
		perror("executor_run: executor is NULL");
		return;
	}

	while (running) {
		pthread_mutex_lock(&cond_mutex);
		if (pthread_cond_wait(&cond, &cond_mutex) != 0) {
			perror("executor_run: pthread_cond_wait failed");
			exit(EXIT_FAILURE);
		}
		pthread_mutex_unlock(&cond_mutex);

		if (channel_is_empty(e->channel)) {
			continue;
		}

		struct task *t = channel_recv(e->channel);

		pthread_mutex_lock(&t->future_mutex);

		enum poll_state state = t->future->poll(t->future, e->channel);

		pthread_mutex_unlock(&t->future_mutex);

		if (state == POLL_READY) {
			task_free(t);
		}
	}
}
