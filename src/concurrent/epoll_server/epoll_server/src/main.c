#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "global.h"
#include "executor.h"
#include "spawner.h"
#include "future.h"
#include "server.h"
#include "wakers.h"
#include "async_listener.h"
#include "io_selector.h"

volatile sig_atomic_t running = 1;

static void handler(int signum)
{
	printf("\nSignal: %d\n", signum);
	running = 0;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, handler);

	// Setup
	struct executor *e = executor_init();

	struct io_selector *selector = io_selector_init(10);
	pthread_t tid = io_selector_spawn(selector);

	struct spawner *spawner1 = executor_get_spawner(e);
	// struct spawner *spawner2 = executor_get_spawner(e);

	// Server
	// spawner_spawn(spawner2, server_init(10000, selector, spawner1));
	spawner_spawn(spawner1, server_init(10000, selector, spawner1));
	executor_run(e);

	// Clean Up
  spawner_free(spawner1);
	pthread_join(tid, NULL);
	io_selector_free(selector);
	executor_free(e);

	return EXIT_SUCCESS;
}
