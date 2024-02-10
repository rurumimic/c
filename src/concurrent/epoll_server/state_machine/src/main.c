#include "future.h"
#include "task.h"

int main(int argc, char *argv[])
{
	struct task *t = init_task();

	while (poll(t->hello) != POLL_READY) {
		/* do nothing */
	}

	free_task(t);

	return 0;
}
