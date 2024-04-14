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
	signal(SIGINT, handler);

	printf("Welcome to the Echo Server.\n");
	printf("Usage: telnet localhost %d\n", PORT);
	printf("Press Ctrl+C to stop the Server.\n");

	// Setup
	struct executor *executor = executor_init();
  if (!executor) {
    return EXIT_FAILURE;
  }

	struct spawner *spawner = executor_get_spawner(executor);

	struct io_selector *selector = io_selector_init(SIZE);
	pthread_t selector_tid = io_selector_spawn(selector);

	// Server
	spawner_spawn(spawner, server_init(PORT, selector, spawner));
	pthread_t executor_tid = executor_spawn(executor);

	// Clean Up
	pthread_join(selector_tid, NULL);

	pthread_cancel(executor_tid);
	pthread_join(executor_tid, NULL);

	io_selector_free(selector);
	spawner_free(spawner);
	executor_free(executor);

	printf("Goodbye.\n");

	return EXIT_SUCCESS;
}
