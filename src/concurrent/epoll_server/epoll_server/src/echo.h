#ifndef _ECHO_H
#define _ECHO_H

#include "future.h"
#include "async_listener.h"

struct echo_data {
  struct async_listener *listener;
  struct async_reader *reader;
  int fd;
  struct future *reader_future;
};

struct future *echo_init(struct async_listener *listener, struct async_reader *reader, int fd);
enum poll_state echo_poll(struct future *f, struct channel *c);
void echo_data_free(struct echo_data *data);

#endif // _ECHO_H
