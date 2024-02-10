#ifndef _SPAWNER_H
#define _SPAWNER_H

#include "hello.h"

struct spawner {
	struct channel *channel;
};

struct spawner *spawner_init(struct channel *channel);
void spawner_spawn(struct spawner *s, struct hello *h);
void spawner_free(struct spawner *s);

#endif // _SPAWNER_H
