#ifndef _SPAWNER_H
#define _SPAWNER_H

#include "../future.h"
#include "channel.h"

struct spawner {
	struct channel *channel;
};

struct spawner *spawner_init(struct channel *channel);
void spawner_free(struct spawner *spawner);
int spawner_spawn(struct spawner *spawner, struct future *future);

#endif // _SPAWNER_H
