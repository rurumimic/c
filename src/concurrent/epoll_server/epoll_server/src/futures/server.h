#ifndef _SERVER_H
#define _SERVER_H

#include "../future.h"
#include "../channel.h"
#include "../io_selector.h"
#include "../spawner.h"
#include "../async_listener.h"

struct server_data {
	// ref
	struct io_selector *selector;
	struct spawner *spawner; // from main

	// owned
	struct async_listener *listener;
};

struct future *server_init(int port, struct io_selector *selector,
			   struct spawner *spawner);
void server_free(struct future *f);

enum poll_state server_poll(struct future *f, struct channel *c);

#endif // _SERVER_H