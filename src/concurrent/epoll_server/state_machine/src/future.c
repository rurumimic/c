#include "future.h"

#include <stdio.h>

enum poll_state poll(struct hello *h)
{
	if (!h) {
		perror("poll: hello is NULL");
		return POLL_READY;
	}

	switch (h->state) {
	case HELLO:
		printf("Hello, ");
		h->state = WORLD;
		return POLL_PENDING;
	case WORLD:
		printf("World!");
		h->state = END;
		return POLL_PENDING;
	default:
		printf("\n");
		return POLL_READY;
	}
}
