#ifndef _SERVER_H
#define _SERVER_H

#include "../future.h"
#include "../channel.h"
#include "../io_selector.h"
#include "../spawner.h"
#include "../async_listener.h"

enum async_server_state {
	SERVER_LISTENING,
};

struct server_data {
	enum async_server_state state;

	// ref
	struct io_selector *selector;
	struct spawner *spawner; // from main

	// owned
	struct async_listener *listener;
};

struct future *server_init(int port, struct io_selector *selector, struct spawner *spawner);
void server_free(struct future *f);
struct poll server_poll(struct future *f, struct context cx);

#endif // _SERVER_H
