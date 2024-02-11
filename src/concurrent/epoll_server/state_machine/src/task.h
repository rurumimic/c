#ifndef _TASK_H
#define _TASK_H

#include "hello.h"

#include <pthread.h>

struct task {
	struct hello *hello;
  pthread_mutex_t mutex;
};

struct task *task_init(void);
void task_free(struct task *t);

#endif // _TASK_H
