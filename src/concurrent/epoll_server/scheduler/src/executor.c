#include "executor.h"
#include "channel.h"
#include "spawner.h"
#include "task.h"
#include "future.h"

#include <stdio.h>
#include <stdlib.h>

struct executor *executor_init(void)
{
	struct executor *e = (struct executor *)malloc(sizeof(struct executor));

	if (!e) {
		perror("executor_init: malloc failed to allocate executor");
		exit(1);
	}

	e->channel = channel_init();

	return e;
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

	while (!channel_is_empty(e->channel)) {
		struct task *t = (struct task *)channel_recv(e->channel);

		if (!t) {
			perror("executor_run: task is NULL");
			exit(EXIT_FAILURE);
		}

		enum poll_state state = poll(t);

		if (state == POLL_READY) {
			task_free((void *)t);
		}
	}
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
