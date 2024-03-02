#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "executor.h"
#include "spawner.h"
#include "future.h"
#include "server.h"
#include "wakers.h"
#include "async_listener.h"
#include "io_selector.h"

int main(int argc, char *argv[])
{
	struct executor *e = executor_init();

	struct io_selector *selector = io_selector_init(10);
	pthread_t tid = io_selector_spawn(selector);

	struct spawner *spawner1 = executor_get_spawner(e);
	struct spawner *spawner2 = executor_get_spawner(e);

	spawner_spawn(spawner2, server_init(10000, selector, spawner1));
	executor_run(e);

	return EXIT_SUCCESS;
}
