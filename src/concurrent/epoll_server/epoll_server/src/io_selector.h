#ifndef _IO_SELECTOR_H
#define _IO_SELECTOR_H

#include "channel.h"
#include "wakers.h"

#include <stddef.h>
#include <pthread.h>
#include <sys/eventfd.h>

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
void *io_selector_select(void *arg);
void io_selector_free(struct io_selector *s);

#endif // _IO_SELECTOR_H
