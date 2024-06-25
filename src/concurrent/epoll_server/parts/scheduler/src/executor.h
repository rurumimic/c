#ifndef _EXECUTOR_H
#define _EXECUTOR_H

#include "channel.h"
#include "spawner.h"

struct executor {
	struct channel *channel;
};

struct executor *executor_init(void);
struct spawner *executor_get_spawner(struct executor *e);
void executor_run(struct executor *e);
void executor_free(struct executor *e);

#endif // _EXECUTOR_H
