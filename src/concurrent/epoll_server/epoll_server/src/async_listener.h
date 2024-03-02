#ifndef _ASYNC_LISTENER_H
#define _ASYNC_LISTENER_H

#include "future.h"
#include "channel.h"
#include "io_selector.h"

struct async_listener {
  int fd;
  struct io_selector *selector;
};

struct accept_data {
  struct async_listener *listener;
  struct async_reader *reader;
  int fd;
  int addr;
  struct waker *waker;
};

struct accept_result {
  struct async_reader *reader;
  int fd;
  int addr;
};

struct async_listener *async_listener_init(int port, struct io_selector *selector);
void async_listener_free(struct async_listener *listener);

struct future *async_listener_accept(struct async_listener *listener);
enum poll_state async_listener_accept_poll(struct future *f, struct channel *c);
void async_listener_accept_data_free(struct accept_data *data);
void async_listener_accept_free(void *p);

#endif // _ASYNC_LISTENER_H
