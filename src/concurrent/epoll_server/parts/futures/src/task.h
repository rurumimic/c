#ifndef _TASK_H
#define _TASK_H

#include <pthread.h>

struct task {
	struct future *future; // async_hello, goodbye
	pthread_mutex_t mutex;
};

struct task *task_init(struct future *future);
void task_free(void *ptr);
void task_wake(void *ptr);

#endif // _TASK_H
