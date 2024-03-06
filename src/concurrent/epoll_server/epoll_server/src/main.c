#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "global.h"
#include "executor.h"
#include "spawner.h"
#include "server.h"
#include "io_selector.h"

#define PORT 10000

volatile sig_atomic_t running = 1;

static void handler(int signum)
{
	printf("Exiting...\n");
	running = 0;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, handler);

	printf("Welcome to the Echo Server.\n");
	printf("Usage: telnet localhost %d\n", PORT);
	printf("Press Ctrl+C to stop the Server.\n");

	// Setup
	struct executor *e = executor_init();
	struct spawner *spawner = executor_get_spawner(e);

	struct io_selector *selector = io_selector_init(10);
	pthread_t tid = io_selector_spawn(selector);

	// Server
	spawner_spawn(spawner, server_init(PORT, selector, spawner));
	executor_run(e);

	// Clean Up
	pthread_join(tid, NULL);
	io_selector_free(selector);
	spawner_free(spawner);
	executor_free(e);

	printf("Goodbye.\n");

	return EXIT_SUCCESS;
}
