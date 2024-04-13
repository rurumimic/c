#ifndef _IO_SELECTOR_H
#define _IO_SELECTOR_H

#include "../future.h"
#include "io_queue.h"
#include "wakers.h"

#include <stddef.h>
#include <pthread.h>
#include <sys/eventfd.h>

struct io_selector {
	int epfd;
	int event;

	struct wakers *wakers;
	struct io_queue *queue;

	pthread_mutex_t wakers_mutex;
	pthread_mutex_t queue_mutex;
};

struct io_selector *io_selector_init(size_t epoll_size);
void io_selector_free(struct io_selector *selector);

pthread_t io_selector_spawn(struct io_selector *selector);

void io_selector_add_event(struct io_selector *selector, uint32_t flags, int fd,
			   struct waker waker, struct wakers *wakers);
void io_selector_remove_event(struct io_selector *selector, int fd,
			      struct wakers *wakers);

void *io_selector_select(void *arg);

void io_selector_register(struct io_selector *selector, uint32_t flags, int fd,
			  struct waker waker);
void io_selector_unregister(struct io_selector *selector, int fd);

#endif // _IO_SELECTOR_H
