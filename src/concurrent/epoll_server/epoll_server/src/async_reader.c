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

struct async_reader *async_reader_init(int fd, struct io_selector *selector)
{
  struct async_reader *r = (struct async_reader *)malloc(sizeof(struct async_reader));

  if (!r) {
    perror("async_reader_init: malloc failed to allocate async_reader");
    exit(EXIT_FAILURE);
  }

  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) {
    perror("async_reader_init: fcntl failed to get flags");
    exit(EXIT_FAILURE);
  }

  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
    perror("async_reader_init: fcntl failed to set flags");
    exit(EXIT_FAILURE);
  }

  r->fd = fd;
  r->selector = selector;

  return r;
}

struct future *async_reader_readline(struct async_reader *reader) {
  struct future *f = (struct future *)malloc(sizeof(struct future));

  if (!f) {
    perror("async_reader_readline: malloc failed to allocate future");
    exit(EXIT_FAILURE);
  }

  f->state = FUTURE_INIT;
  f->poll = async_reader_poll;
  f->data = reader;

  printf("async_reader_readline: future initialized\n");

  return f;
}

enum poll_state async_reader_poll(struct future *f, struct channel *c)
{
  struct async_reader *r = (struct async_reader *)f->data;
  char buf[1024] = {0, };

  errno = 0;
  printf("read...\n");
  ssize_t n = read(r->fd, buf, sizeof(buf) - 1);

  if (n < 0) {
    if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
      struct task *t = task_init(f, c);
      struct waker *waker = waker_init(t);
      io_selector_register(r->selector, EPOLLIN, r->fd, waker);
      return POLL_PENDING;
    } else {
      return POLL_READY;
    }
  }

  if (n == 0) {
    return POLL_READY;
  }

  // printf("read: %s\n", buf);
  // memcpy(r->lines, buf, 1024);

  // memcpy()
  // memcpy(result.value.read_result, buf, 1024);

  return POLL_READY;
}

void async_reader_free(void *p)
{
  if (!p) {
    perror("async_reader_free: async_reader is NULL");
    return;
  }

  struct async_reader *r = (struct async_reader *)p;

  free(r);
}

