#ifndef _FUTURE_H
#define _FUTURE_H

#include "poll.h"

struct waker {
	void *ptr; // == task
	void (*wake)(void *data); // channel_send(task)
	void (*free)(void *data); // free(task)
};

struct context {
	struct waker waker;
};

struct future {
	void *data; // server, accept, echo, readline
	struct poll (*poll)(struct future *f, struct context cx);
	void (*free)(struct future *f);
};

struct context from_waker(struct waker waker);

#endif // _FUTURE_H
