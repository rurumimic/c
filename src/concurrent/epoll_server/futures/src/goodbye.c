#include "goodbye.h"
#include "future.h"
#include "poll.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct future *goodbye_init(void)
{
	struct future *f = (struct future *)malloc(sizeof(struct future));
	if (!f) {
		perror("goodbye: malloc failed to allocate future");
		exit(EXIT_FAILURE);
	}

	struct goodbye *g = (struct goodbye *)malloc(sizeof(struct goodbye));

	if (!g) {
		perror("goodbye: malloc failed to allocate goodbye");
		exit(EXIT_FAILURE);
	}

	char *s = (char *)calloc(GOODBYE_SIZE, sizeof(char));
	if (!s) {
		perror("goodbye: malloc failed to allocate string");
		exit(EXIT_FAILURE);
	}

	strcpy(s, "Goodbye.");
	g->s = s;

	f->data = g;
	f->poll = goodbye_poll;
	f->free = goodbye_free;

	return f;
}

void goodbye_free(struct future *future)
{
	if (!future) {
		perror("goodbye_free: future is NULL");
		return;
	}

	struct goodbye *g = (struct goodbye *)future->data;

	if (g) {
		if (g->s) {
			free(g->s);
		}
		free(g);
	}

	free(future);
}

struct poll goodbye_poll(struct future *future, struct context cx)
{
	struct goodbye *g = (struct goodbye *)future->data;

	printf("[Goodbye] Ready\n");

	return (struct poll){ .state = POLL_READY,
			      .output = g->s,
			      .free = NULL };
}
