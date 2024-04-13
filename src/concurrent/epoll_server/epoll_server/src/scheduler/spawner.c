#include "../future.h"
#include "../global.h"
#include "channel.h"
#include "spawner.h"
#include "task.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct spawner *spawner_init(struct channel *channel)
{
	if (!channel) {
		perror("spawner_init: channel is NULL");
		return NULL;
	}

	struct spawner *spawner = (struct spawner *)malloc(sizeof(struct spawner));

	if (!spawner) {
		perror("spawner_init: malloc failed to allocate spawner");
		exit(EXIT_FAILURE);
	}

	spawner->channel = channel;

	return spawner;
}

void spawner_free(struct spawner *spawner)
{
	if (!spawner) {
		perror("spawner_free: spawner is NULL");
		return;
	}

	free(spawner);
}

void spawner_spawn(struct spawner *spawner, struct future *future)
{
	if (!spawner) {
		perror("spawner_spawn: spawner is NULL");
		return;
	}

	if (!future) {
		perror("spawner_spawn: future is NULL");
		return;
	}

	struct task *task = task_init(future, spawner->channel);

	pthread_mutex_lock(&cond_mutex);
	channel_send(spawner->channel, task);
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&cond_mutex);
}
