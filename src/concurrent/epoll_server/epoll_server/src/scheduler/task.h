#ifndef _TASK_H
#define _TASK_H

#include "../future.h"
#include "channel.h"

#include <pthread.h>

struct task {
	struct future *future; // server, accept, echo, readline
	struct channel *channel;
	pthread_mutex_t mutex;
};

struct task *task_init(struct future *f, struct channel *c);
void task_free(void *ptr);
void task_wake(void *ptr);

#endif // _TASK_H
