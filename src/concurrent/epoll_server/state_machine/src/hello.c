#include "hello.h"

#include <stdlib.h>

struct hello *init_hello(void)
{
	struct hello *h = (struct hello *)malloc(sizeof(struct hello));
	h->state = HELLO;
	return h;
}

void free_hello(struct hello *h)
{
	if (h) {
		free(h);
	}
}
