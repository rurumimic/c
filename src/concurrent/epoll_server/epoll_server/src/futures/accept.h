#ifndef _ACCEPT_H
#define _ACCEPT_H

#include <netinet/in.h>

#include "../future.h"
#include "../scheduler/io_selector.h"

struct accept_data {
	// ref
	struct io_selector *selector;
	int sfd;

	// owned
	int cfd;
	char cip[INET_ADDRSTRLEN];
};

struct future *accept_init(struct io_selector *selector, int sfd);
void accept_free(struct future *future);
struct poll accept_poll(struct future *future, struct context context);

#endif // _ACCEPT_H
