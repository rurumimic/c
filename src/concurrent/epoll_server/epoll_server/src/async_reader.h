#ifndef _ASYNC_READER_H
#define _ASYNC_READER_H

struct future;
struct io_selector;
struct channel;

struct async_reader {
  int fd;
  struct io_selector *selector;
  char lines[1024];
};

struct reader_data {
  struct async_reader *reader;
  char *lines;
};

struct reader_result {
  char *lines;
};

struct async_reader *async_reader_init(int fd, struct io_selector *selector);
struct future *async_reader_readline(struct async_reader *reader);
enum poll_state async_reader_poll(struct future *f, struct channel *c);
void async_reader_free(void *p);

#endif // !_ASYNC_READER_H
