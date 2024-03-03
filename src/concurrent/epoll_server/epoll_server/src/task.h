#ifndef _TASK_H
#define _TASK_H

#include "future.h"
#include "channel.h"

#include <pthread.h>

struct task {
	struct future *future;
	struct channel *channel;
	pthread_mutex_t future_mutex;
};

struct task *task_init(struct future *f, struct channel *c);
void task_wake_by_ref(struct task *t);
void task_free(struct task *t);

#endif // _TASK_H
