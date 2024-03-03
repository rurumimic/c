#ifndef _SERVER_H
#define _SERVER_H

#include "future.h"
#include "channel.h"
#include "io_selector.h"
#include "spawner.h"

struct server_data {
  struct spawner *spawner; // from main
  struct async_listener *listener;
  struct future *accept;
};

struct future *server_init(int port, struct io_selector *selector, struct spawner *spawner);
enum poll_state server_poll(struct future *f, struct channel *c);
void server_data_free(struct server_data *data);

#endif // _SERVER_H
