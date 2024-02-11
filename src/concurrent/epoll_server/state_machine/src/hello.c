#include "hello.h"

#include <stdio.h>
#include <stdlib.h>

struct hello *hello_init(void)
{
	struct hello *h = (struct hello *)malloc(sizeof(struct hello));

	if (!h) {
		perror("hello_init: malloc failed to allocate hello");
		exit(EXIT_FAILURE);
	}

	h->state = HELLO;
	return h;
}

void hello_free(struct hello *h)
{
	if (!h) {
		perror("hello_free: hello is NULL");
		return;
	}

	free(h);
}
