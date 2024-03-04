#ifndef _ASYNC_LISTENER_H
#define _ASYNC_LISTENER_H

#include <netinet/in.h>

#include "future.h"
#include "channel.h"
#include "io_selector.h"

struct async_listener {
  // ref
  struct io_selector *selector;

  // owned
  int sfd;
};

struct accept_data {
  // ref
  struct io_selector *selector;
  struct future *server; // waker
  int sfd;

  // owned
  int cfd;
  char cip[INET_ADDRSTRLEN];
};

struct async_listener *async_listener_init(int port, struct io_selector *selector);
void async_listener_free(struct async_listener *listener);

struct future *async_listener_accept(struct future *server, struct async_listener *listener);
enum poll_state async_listener_accept_poll(struct future *f, struct channel *c);
void async_listener_accept_free(struct future *f);

#endif // _ASYNC_LISTENER_H
