#include "future.h"
#include "task.h"

int main(int argc, char *argv[])
{
	struct task *t = task_init();
	pthread_mutex_lock(&t->mutex);

	while (poll(t->hello) != POLL_READY) {
		/* do nothing */
	}

	pthread_mutex_unlock(&t->mutex);
	task_free(t);

	return 0;
}
