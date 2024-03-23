#include "future.h"
#include "task.h"

#include <stdio.h>

enum poll_state poll(struct task *t)
{
	if (!t) {
		perror("poll: task is NULL");
		return POLL_READY;
	}

	struct hello *h = t->hello;

	switch (h->state) {
	case HELLO:
		printf("Hello, ");
		h->state = WORLD;
		task_wake_by_ref(t);
		return POLL_PENDING;
	case WORLD:
		printf("World!");
		h->state = END;
		task_wake_by_ref(t);
		return POLL_PENDING;
	default:
		printf("\n");
		return POLL_READY;
	}
}
