#include "async_hello.h"
#include "future.h"
#include "poll.h"
#include "goodbye.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct future *hello_init(void)
{
	struct future *f = (struct future *)malloc(sizeof(struct future));
	if (!f) {
		perror("hello_init: malloc failed to allocate future");
		exit(EXIT_FAILURE);
	}

	struct hello *h = (struct hello *)malloc(sizeof(struct hello));

	if (!h) {
		perror("hello_init: malloc failed to allocate hello");
		exit(EXIT_FAILURE);
	}

	h->async_state = HELLO;
	h->goodbye = goodbye_init();

	f->data = h;
	f->poll = hello_poll;
	f->free = hello_free;

	return f;
}

void hello_free(struct future *future)
{
	if (!future) {
		perror("hello_free: future is NULL");
		return;
	}

	struct hello *h = (struct hello *)future->data;

	if (h) {
		if (h->goodbye) {
			h->goodbye->free(h->goodbye);
		}
		free(h);
	}

	free(future);
}

struct poll hello_poll(struct future *future, struct context cx)
{
	struct hello *h = (struct hello *)future->data;
	struct poll goodbye_poll;

	switch (h->async_state) {
	case HELLO:
		printf("[Hello] Pending: Hello\n");
		h->goodbye->poll(h->goodbye, cx);
		h->async_state = WORLD;
		return (struct poll){ .state = POLL_PENDING,
				      .output = NULL,
				      .free = NULL };
	case WORLD:
		printf("[Hello] Pending: Hello, World\n");
		h->goodbye->poll(h->goodbye, cx);
		h->async_state = END;
		return (struct poll){ .state = POLL_PENDING,
				      .output = NULL,
				      .free = NULL };
	default:
		printf("[Hello] READY: Hello, World!!!\n");
		goodbye_poll = h->goodbye->poll(h->goodbye, cx);
		if (goodbye_poll.state == POLL_READY) {
			char *output = goodbye_poll.output;
			return (struct poll){
				.state = POLL_READY,
				.output = (void *)output,
				.free = goodbye_poll.free,
			};
		} else {
			// unreachable
			char *output = (char *)malloc(sizeof(char) * 50);
			strcpy(output, "[Hello] But Goodbye is not ready.\n");
			return (struct poll){ .state = POLL_READY,
					      .output = output,
					      .free = free };
		}
	}
}
