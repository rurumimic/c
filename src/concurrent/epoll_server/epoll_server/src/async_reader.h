#ifndef _ASYNC_READER_H
#define _ASYNC_READER_H

#include <stddef.h>

struct future;
struct channel;
struct io_selector;

struct async_reader {
  int cfd;
  struct io_selector *selector;
};

struct readline_data {
  struct future *echo;
  struct async_reader *reader;
  char lines[1024];
  size_t len;
};

struct async_reader *async_reader_init(int cfd, struct io_selector *selector);
struct future *async_reader_readline(struct future *echo, struct async_reader *reader);
enum poll_state async_reader_poll(struct future *f, struct channel *c);
void async_reader_free(struct async_reader *reader);

#endif // !_ASYNC_READER_H
