#include "spawner.h"
#include "task.h"
#include "hello.h"
#include "channel.h"

#include <stdio.h>
#include <stdlib.h>

struct spawner *spawner_init(struct channel *channel)
{
	struct spawner *s = (struct spawner *)malloc(sizeof(struct spawner));

	if (!s) {
		perror("spawner_init: malloc failed to allocate spawner");
		exit(EXIT_FAILURE);
	}

	s->channel = channel;

	return s;
}

void spawner_spawn(struct spawner *s, struct hello *h)
{
	if (!s) {
		perror("spawner_spawn: spawner is NULL");
		return;
	}

	if (!h) {
		perror("spawner_spawn: hello is NULL");
		return;
	}

	struct task *t = task_init(h, s->channel);

	channel_send(s->channel, sizeof(struct task), t, task_free);
}

void spawner_free(struct spawner *s)
{
	if (!s) {
		perror("spawner_free: spawner is NULL");
		return;
	}

	free(s);
}
