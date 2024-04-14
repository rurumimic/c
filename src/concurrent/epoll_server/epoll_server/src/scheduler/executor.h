#ifndef _EXECUTOR_H
#define _EXECUTOR_H

#include "channel.h"
#include "spawner.h"

#include <pthread.h>

struct executor {
	struct channel *channel;
};

struct executor *executor_init(void);
void executor_free(struct executor *executor);

struct spawner *executor_get_spawner(struct executor *executor);

int executor_spawn(struct executor *executor,
		   pthread_t *__restrict __newthread);
void executor_cancel(void *arg);
void *executor_run(void *arg);

#endif // _EXECUTOR_H
