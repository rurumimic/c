#include "task.h"
#include "hello.h"

#include <stdlib.h>

struct task *init_task(void)
{
	struct task *t = (struct task *)malloc(sizeof(struct task));
	t->hello = init_hello();
	return t;
}

void free_task(struct task *t)
{
	if (t) {
		if (t->hello) {
			free_hello(t->hello);
		}
		free(t);
	}
}
