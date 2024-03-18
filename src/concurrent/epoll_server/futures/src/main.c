#include "future.h"
#include "task.h"
#include "async_hello.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
	struct future *f = hello_init();
	struct task *t = task_init(f);

	struct waker waker = { .ptr = t, .wake = task_wake, .free = task_free };
	struct context cx = from_waker(waker);

	while (1) {
		pthread_mutex_lock(&t->mutex);
		struct poll poll = t->future->poll(t->future, cx);
		pthread_mutex_unlock(&t->mutex);

		if (poll.state == POLL_READY) {
			printf("[Main] %s\n", (char *)poll.output);
			if (poll.free) {
				poll.free(poll.output);
			}
			break;
		}
	}

	task_free(t);

	return 0;
}
