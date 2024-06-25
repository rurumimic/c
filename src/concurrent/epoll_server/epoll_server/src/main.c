#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#include "global.h"
#include "futures/server.h"
#include "scheduler/executor.h"
#include "scheduler/spawner.h"
#include "scheduler/io_selector.h"

#define PORT 10000
#define SIZE 10

volatile sig_atomic_t running = 1;

static void handler(int signum)
{
	printf("[SIG: %d]\n", signum);
	running = 0;
}

int main(int argc, char *argv[])
{
	pthread_t selector_tid;
	pthread_t executor_tid;

	signal(SIGINT, handler);

	printf("Welcome to the Echo Server.\n");
	printf("Usage: telnet localhost %d\n", PORT);
	printf("Press Ctrl+C to stop the Server.\n");

	// Setup
	struct executor *executor = executor_init();
	struct spawner *spawner = executor_get_spawner(executor);

	struct io_selector *selector = io_selector_init(SIZE);
	io_selector_spawn(selector, &selector_tid);

	// Server
	spawner_spawn(spawner, server_init(PORT, selector, spawner));
	executor_spawn(executor, &executor_tid);

	// Clean Up
	pthread_join(selector_tid, NULL);

	pthread_cancel(executor_tid);
	pthread_join(executor_tid, NULL);

	spawner_free(spawner);
	executor_free(executor);
	io_selector_free(selector);

	printf("Goodbye.\n");

	return EXIT_SUCCESS;
}
