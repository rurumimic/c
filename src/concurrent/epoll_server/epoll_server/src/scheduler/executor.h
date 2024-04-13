#ifndef _EXECUTOR_H
#define _EXECUTOR_H

#include "channel.h"
#include "spawner.h"

struct executor {
	struct channel *channel;
};

struct executor *executor_init(void);
void executor_free(struct executor *executor);

struct spawner *executor_get_spawner(struct executor *executor);
void executor_run(struct executor *executor);

#endif // _EXECUTOR_H
