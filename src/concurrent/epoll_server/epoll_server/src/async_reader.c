#include "async_reader.h"
#include "future.h"
#include "channel.h"
#include "io_selector.h"

#include <sys/epoll.h>
#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

struct async_reader *async_reader_init(int cfd, struct io_selector *selector)
{
  struct async_reader *r = (struct async_reader *)malloc(sizeof(struct async_reader));

  if (!r) {
    perror("async_reader_init: malloc failed to allocate async_reader");
    exit(EXIT_FAILURE);
  }

  int flags = fcntl(cfd, F_GETFL, 0);
  if (flags < 0) {
    perror("async_reader_init: fcntl failed to get flags");
    exit(EXIT_FAILURE);
  }

  if (fcntl(cfd, F_SETFL, flags | O_NONBLOCK) < 0) {
    perror("async_reader_init: fcntl failed to set flags");
    exit(EXIT_FAILURE);
  }

  r->cfd = cfd;
  r->selector = selector;

  printf("async_reader_init: cfd: %d\n", cfd);

  return r;
}

struct future *async_reader_readline(struct future *echo, struct async_reader *reader) {
  struct future *f = (struct future *)malloc(sizeof(struct future));

  if (!f) {
    perror("async_reader_readline: malloc failed to allocate future");
    exit(EXIT_FAILURE);
  }

  struct readline_data *data = (struct readline_data *)malloc(sizeof(struct readline_data));

  if (!data) {
    perror("async_reader_readline: malloc failed to allocate readline_data");
    exit(EXIT_FAILURE);
  }

  data->echo = echo;
  data->reader = reader;
  memset(data->lines, 0, 1024);

  f->state = FUTURE_INIT;
  f->poll = async_reader_poll;
  f->data = data;

  // printf("async_reader_readline: f: %p\n", f);

  return f;
}

enum poll_state async_reader_poll(struct future *f, struct channel *c)
{
  struct readline_data *data = (struct readline_data *)f->data;
  struct async_reader *r = data->reader;
  struct future *echo = data->echo;
  char buf[1024] = {0, };

  errno = 0;
  ssize_t n = read(r->cfd, buf, sizeof(buf) - 1);

  if (n < 0) {
    if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
      io_selector_register(r->selector, EPOLLIN, r->cfd, task_init(echo, c));
      return POLL_PENDING;
    } else {
      data->len = -1;
      return POLL_READY; // error
    }
  }

  if (n == 0) {
    data->len = 0;
    return POLL_READY;
  }

  data->len = n;
  memcpy(data->lines, buf, 1024);
  printf("poll read: %s\n", data->lines);

  return POLL_READY;
}

void async_reader_free(struct async_reader *reader)
{
  if (!reader) {
    perror("async_reader_free: async_reader is NULL");
    return;
  }

  free(reader);
}

