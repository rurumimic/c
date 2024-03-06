#ifndef _ACCEPT_H
#define _ACCEPT_H

#include <netinet/in.h>

#include "../async_listener.h"
#include "../future.h"
#include "../channel.h"
#include "../io_selector.h"

struct accept_data {
	// ref
	struct io_selector *selector;
	struct future *server; // waker
	int sfd;

	// owned
	int cfd;
	char cip[INET_ADDRSTRLEN];
};

struct future *accept_init(struct future *server,
			   struct async_listener *listener);
void accept_free(struct future *f);

enum poll_state accept_poll(struct future *f, struct channel *c);

#endif // _ACCEPT_H
