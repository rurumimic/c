#ifndef _SERVER_H
#define _SERVER_H

#include "future.h"
#include "channel.h"
#include "io_selector.h"
#include "spawner.h"

struct server_data {
  // ref
  struct io_selector *selector;
  struct spawner *spawner; // from main

  // owned
  struct async_listener *listener;
};

struct future *server_init(int port, struct io_selector *selector, struct spawner *spawner);
enum poll_state server_poll(struct future *f, struct channel *c);
void server_free(struct future *f);

#endif // _SERVER_H
