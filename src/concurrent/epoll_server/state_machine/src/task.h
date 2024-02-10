#ifndef _TASK_H
#define _TASK_H

#include "hello.h"

struct task {
	struct hello *hello;
};

struct task *init_task(void);
void free_task(struct task *task);

#endif // _TASK_H
