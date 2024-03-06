#include "spawner.h"
#include "task.h"
#include "future.h"
#include "channel.h"

#include <stdio.h>
#include <stdlib.h>

struct spawner *spawner_init(struct channel *c)
{
	if (!c) {
		perror("spawner_init: channel is NULL");
		return NULL;
	}

	struct spawner *s = (struct spawner *)malloc(sizeof(struct spawner));

	if (!s) {
		perror("spawner_init: malloc failed to allocate spawner");
		exit(EXIT_FAILURE);
	}

	s->channel = c;

	return s;
}

void spawner_free(struct spawner *s)
{
	if (!s) {
		perror("spawner_free: spawner is NULL");
		return;
	}

	free(s);
}

void spawner_spawn(struct spawner *s, struct future *f)
{
	if (!s) {
		perror("spawner_spawn: spawner is NULL");
		return;
	}

	if (!f) {
		perror("spawner_spawn: future is NULL");
		return;
	}

	struct task *t = task_init(f, s->channel);

	channel_send(s->channel, t);
}
