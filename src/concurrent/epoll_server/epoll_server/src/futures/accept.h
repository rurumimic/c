#ifndef _ACCEPT_H
#define _ACCEPT_H

#include <netinet/in.h>

#include "../future.h"
#include "../io_selector.h"

struct accept_data {
	// ref
	struct io_selector *selector;
	// struct future *server; // waker
	int sfd;

	// owned
	int cfd;
	char cip[INET_ADDRSTRLEN];
};

struct future *accept_init(struct io_selector *selector, int sfd);
void accept_free(struct future *f);
struct poll accept_poll(struct future *f, struct context cx);

#endif // _ACCEPT_H
