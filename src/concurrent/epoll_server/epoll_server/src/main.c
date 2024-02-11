#include <stdio.h>
#include <stdlib.h>

#include "executor.h"
#include "spawner.h"
#include "hello.h"
#include "wakers.h"

int main(int argc, char *argv[])
{
	struct wakers *w = wakers_init(3);

	if (!w) {
		perror("main: failed to initialize wakers");
		return EXIT_FAILURE;
	}

	wakers_free(w);

	return EXIT_SUCCESS;
}
