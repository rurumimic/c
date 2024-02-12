#ifndef _IO_SELECTOR_H
#define _IO_SELECTOR_H

#include "channel.h"
#include "wakers.h"

#include <stddef.h>
#include <pthread.h>
#include <sys/eventfd.h>

enum io_ops_type {
  IO_OPS_ADD,
  IO_OPS_REMOVE,
};

struct io_ops {
  enum io_ops_type type;
  int flags;
  int fd;
  struct waker *waker;
};

struct io_selector {
	int epfd;
  int event;

	struct wakers *wakers;
	struct channel *queue;

	pthread_mutex_t wakers_mutex;
	pthread_mutex_t queue_mutex;
};

struct io_selector *io_selector_init(size_t epoll_size);
pthread_t io_selector_spawn(struct io_selector *s);
void io_selector_add_event(struct io_selector *s, uint32_t flags, int fd, struct waker *waker, struct wakers *wakers);
void io_selector_remove_event(struct io_selector *s, int fd, struct wakers *wakers);
void *io_selector_select(void *arg);
void io_selector_register(struct io_selector *s, uint32_t flags, int fd, struct waker *waker);
void io_selector_unregister(struct io_selector *s, int fd);
void io_selector_free(struct io_selector *s);

void io_ops_free(void *p);

#endif // _IO_SELECTOR_H
