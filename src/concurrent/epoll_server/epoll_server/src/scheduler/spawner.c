#include "../future.h"
#include "../global.h"
#include "channel.h"
#include "spawner.h"
#include "task.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct spawner *spawner_init(struct channel *channel)
{
	assert(channel != NULL);

	struct spawner *spawner =
		(struct spawner *)malloc(sizeof(struct spawner));

	if (!spawner) {
		perror("spawner_init: malloc failed to allocate spawner");
		exit(EXIT_FAILURE);
	}

	spawner->channel = channel;

	return spawner;
}

void spawner_free(struct spawner *spawner)
{
	assert(spawner != NULL);

	free(spawner);
}

void spawner_spawn(struct spawner *spawner, struct future *future)
{
	assert(spawner != NULL);
	assert(future != NULL);

	struct task *task = task_init(future, spawner->channel);

	channel_send(spawner->channel, task);
}
