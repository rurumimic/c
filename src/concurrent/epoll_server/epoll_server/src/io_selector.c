#include "io_selector.h"
#include "wakers.h"
#include "channel.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <pthread.h>

struct io_selector *io_selector_init(size_t epoll_size)
{
	struct io_selector *s =
		(struct io_selector *)malloc(sizeof(struct io_selector));

	if (!s) {
		perror("io_selector_init: malloc failed to allocate io_selector");
		exit(EXIT_FAILURE);
	}

	s->epfd = epoll_create(epoll_size);

	if (s->epfd == -1) {
		perror("io_selector_init: epoll_create failed to create epoll instance");
		exit(EXIT_FAILURE);
	}

	s->event = eventfd(0, 0);

	if (s->event == -1) {
		perror("io_selector_init: eventfd failed to create eventfd instance");
		exit(EXIT_FAILURE);
	}

	s->wakers = wakers_init(epoll_size);
	s->queue = channel_init();

	pthread_mutex_init(&s->wakers_mutex, NULL);
	pthread_mutex_init(&s->queue_mutex, NULL);

	return s;
}

pthread_t io_selector_spawn(struct io_selector *s)
{
	pthread_t tid;

	if (pthread_create(&tid, NULL, io_selector_select, (void *)s) != 0) {
		perror("io_selector_run: pthread_create failed to create io_selector thread");
		exit(EXIT_FAILURE);
	}

	return tid;
}

void io_selector_add_event(struct io_selector *s, uint32_t flags, int fd, struct waker *waker, struct wakers *wakers)
{
	struct epoll_event ev;

	ev.events = flags | EPOLLONESHOT;
	ev.data.fd = fd;

	errno = 0;

	if (!epoll_ctl(s->epfd, EPOLL_CTL_ADD, fd, &ev)) {
		perror("io_selector_add_event: epoll_ctl failed to add event to epoll instance");

		if (errno == EEXIST) { // already exists
			if (!epoll_ctl(s->epfd, EPOLL_CTL_MOD, fd, &ev)) {
				perror("io_selector_add_event: epoll_ctl failed to modify event in epoll instance");
				exit(EXIT_FAILURE);
			}
		} else {
			perror("io_selector_add_event: epoll_ctl failed to add event to epoll instance");
			exit(EXIT_FAILURE);
		}
	}

	if (wakers_find(wakers, fd)) {
		perror("io_selector_add_event: wakers_find already have waker for fd");
		exit(EXIT_FAILURE);
	}

	wakers_insert(wakers, fd, waker);
}

void io_selector_remove_event(struct io_selector *s, int fd,
			      struct wakers *wakers)
{
	struct epoll_event ev;

	ev.events = 0;
	ev.data.fd = fd;

	if (!epoll_ctl(s->epfd, EPOLL_CTL_DEL, fd, &ev)) {
		perror("io_selector_remove_event: epoll_ctl failed to remove event from epoll instance");
		exit(EXIT_FAILURE);
	}

	struct waker *waker = wakers_remove(wakers, fd);

	if (!waker) {
		perror("io_selector_remove_event: wakers_remove failed to remove waker");
		exit(EXIT_FAILURE);
	}

	waker_free(waker);
}

void *io_selector_select(void *arg)
{
	struct io_selector *s = (struct io_selector *)arg;

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = s->event;

	if (!epoll_ctl(s->epfd, EPOLL_CTL_ADD, s->event, &ev)) {
		perror("io_selector_select: epoll_ctl failed to add event to epoll instance");
		exit(EXIT_FAILURE);
	}

	struct epoll_event *events = (struct epoll_event *)malloc(
		s->wakers->capacity * sizeof(struct epoll_event));
	if (!events) {
		perror("io_selector_select: malloc failed to allocate events");
		exit(EXIT_FAILURE);
	}

	while (1) {
		int n = epoll_wait(s->epfd, events, s->wakers->capacity, -1);

		if (n == -1) {
			perror("io_selector_select: epoll_wait failed to wait for events");
			exit(EXIT_FAILURE);
		}

		pthread_mutex_lock(&s->wakers_mutex);
		for (int i = 0; i < n; i++) {
			if (events[i].data.fd == s->event) {
				pthread_mutex_lock(&s->queue_mutex);
				while (!channel_is_empty(s->queue)) {
					struct io_ops *ops = (struct io_ops *)channel_recv(s->queue);
					if (ops->type == IO_OPS_ADD) {
						io_selector_add_event(s, ops->flags, ops->fd, ops->waker, s->wakers);
					} else if (ops->type == IO_OPS_REMOVE) {
						io_selector_remove_event(s, ops->fd, s->wakers);
					}
          free(ops);
				}
				pthread_mutex_unlock(&s->queue_mutex);
			} else {
				struct waker *waker = wakers_remove(s->wakers, events[i].data.fd);
				if (waker) {
					waker->wake_by_ref(waker->data);
				}
			}
		}
		pthread_mutex_unlock(&s->wakers_mutex);
	}

	return NULL;
}

void io_selector_register(struct io_selector *s, uint32_t flags, int fd, struct waker *waker)
{
	pthread_mutex_lock(&s->queue_mutex);

	struct io_ops *ops = (struct io_ops *)malloc(sizeof(struct io_ops));
	if (!ops) {
		perror("io_selector_register: malloc failed to allocate io_ops");
		exit(EXIT_FAILURE);
	}

	ops->type = IO_OPS_ADD;
	ops->flags = flags;
	ops->fd = fd;
	ops->waker = waker;

	channel_send(s->queue, sizeof(struct io_ops), ops, io_ops_free);

	eventfd_write(s->event, 1);

	pthread_mutex_unlock(&s->queue_mutex);
}

void io_selector_unregister(struct io_selector *s, int fd)
{
	pthread_mutex_lock(&s->queue_mutex);

	struct io_ops *ops = (struct io_ops *)malloc(sizeof(struct io_ops));
	if (!ops) {
		perror("io_selector_unregister: malloc failed to allocate io_ops");
		exit(EXIT_FAILURE);
	}

	ops->type = IO_OPS_REMOVE;
	ops->flags = 0;
	ops->fd = fd;
	ops->waker = NULL;

	channel_send(s->queue, sizeof(struct io_ops), ops, io_ops_free);

	eventfd_write(s->event, 1);

	pthread_mutex_unlock(&s->queue_mutex);
}

void io_ops_free(void *p) {
  if (!p) {
    perror("io_ops_free: io_ops is NULL");
    return;
  }

  struct io_ops *ops = (struct io_ops *)p;
  waker_free(ops->waker);
  free(ops);
}

