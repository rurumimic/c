#ifndef _FUTURE_H
#define _FUTURE_H

#include "poll.h"

struct waker {
	void *ptr; // => task
	void (*wake)(void *data); // queue.push(ptr)
	void (*free)(void *data);
};

struct context {
	struct waker waker;
};

struct future {
	void *data; // => async_hello
	struct poll (*poll)(struct future *future, struct context cx);
	void (*free)(struct future *future);
};

struct context from_waker(struct waker waker);

#endif // _FUTURE_H
