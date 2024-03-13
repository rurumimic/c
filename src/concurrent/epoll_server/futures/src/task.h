#ifndef _TASK_H
#define _TASK_H

#include <pthread.h>

struct task {
	struct future *future;
  pthread_mutex_t mutex;
};

struct task *task_init(void);
void task_free(struct task *t);

#endif // _TASK_H
