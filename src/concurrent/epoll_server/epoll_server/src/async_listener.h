#ifndef _ASYNC_LISTENER_H
#define _ASYNC_LISTENER_H

#include <netinet/in.h>

#include "io_selector.h"

struct async_listener {
	// ref
	struct io_selector *selector;

	// owned
	int sfd;
};

struct async_listener *async_listener_init(int port, struct io_selector *selector);
void async_listener_free(struct async_listener *listener);
struct future *async_listener_accept(struct async_listener *listener);

#endif // _ASYNC_LISTENER_H
