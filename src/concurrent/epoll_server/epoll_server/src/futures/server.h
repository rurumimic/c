#ifndef _SERVER_H
#define _SERVER_H

#include "../future.h"
#include "../io/async_listener.h"
#include "../scheduler/channel.h"
#include "../scheduler/io_selector.h"
#include "../scheduler/spawner.h"

enum async_server_state {
	SERVER_LISTENING,
	SERVER_ACCEPTED,
	SERVER_FINISHED,
};

struct server_data {
	// owned
	enum async_server_state state;
	struct async_listener *listener;

	// ref
	struct io_selector *selector;
	struct spawner *spawner; // from main
};

struct future *server_init(int port, struct io_selector *selector,
			   struct spawner *spawner);
void server_free(struct future *future);
struct poll server_poll(struct future *future, struct context context);

#endif // _SERVER_H
