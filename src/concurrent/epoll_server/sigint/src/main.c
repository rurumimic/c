#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static volatile sig_atomic_t running = 1;

static void handler(int signum)
{
	printf("\nSignal: %d\n", signum);
	running = 0;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, handler);

	printf("Start\n");

  int count = 1;

	while (running) {
		printf("%d\n", count++);
		sleep(1);
	}

	printf("Exit.\n");

	return 0;
}
