#ifndef _ASYNC_HELLO_H
#define _ASYNC_HELLO_H

#include "future.h"
#include "poll.h"

enum async_hello_state {
	HELLO,
	WORLD,
	END,
};

struct hello {
	enum async_hello_state async_state;
	struct future *goodbye;
};

struct future *hello_init(void);
void hello_free(struct future *future);
struct poll hello_poll(struct future *future, struct context cx);

#endif // _ASYNC_HELLO_H
