#ifndef _SPAWNER_H
#define _SPAWNER_H

#include "channel.h"
#include "future.h"

struct spawner {
	struct channel *channel;
};

struct spawner *spawner_init(struct channel *c);
void spawner_free(struct spawner *s);

void spawner_spawn(struct spawner *s, struct future *f);

#endif // _SPAWNER_H
