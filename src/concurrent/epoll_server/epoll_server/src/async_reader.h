#ifndef _ASYNC_READER_H
#define _ASYNC_READER_H

#include <stddef.h>

#define MAX_CHARS 1024

struct future;
struct channel;
struct io_selector;

struct async_reader {
  // ref
  struct io_selector *selector;
  int cfd;
};

struct readline_data {
  // ref
  struct io_selector *selector;
  struct future *echo;
  int cfd;

  // owned
  char lines[1024];
  size_t len;
};

struct async_reader *async_reader_init(struct io_selector *selector, int cfd);
void async_reader_free(struct async_reader *reader);

struct future *async_reader_readline(struct future *echo, struct io_selector *selector, int cfd);
enum poll_state async_reader_readline_poll(struct future *f, struct channel *c);
void async_reader_readline_free(struct future *f);

#endif // !_ASYNC_READER_H
