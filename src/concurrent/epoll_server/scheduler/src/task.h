#ifndef _TASK_H
#define _TASK_H

#include "hello.h"
#include "channel.h"

#include <pthread.h>

struct task {
	struct hello *hello;
	struct channel *channel;
	pthread_mutex_t mutex;
};

struct task *task_init(struct hello *h, struct channel *c);
void task_wake_by_ref(struct task *t);
void task_free(void *p);

#endif // _TASK_H
