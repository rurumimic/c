#include "../global.h"
#include "io_selector.h"
#include "io_queue.h"
#include "task.h"
#include "wakers.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <unistd.h>

struct io_selector *io_selector_init(size_t epoll_size)
{
	struct io_selector *selector =
		(struct io_selector *)malloc(sizeof(struct io_selector));

	if (!selector) {
		perror("io_selector_init: malloc failed to allocate io_selector");
		return NULL;
	}

	int epfd = epoll_create(epoll_size);
	if (epfd == -1) {
		perror("io_selector_init: epoll_create failed to create epoll instance");
		free(selector);
		return NULL;
	}

	int event = eventfd(0, 0);
	if (event == -1) {
		perror("io_selector_init: eventfd failed to create eventfd instance");
		close(epfd);
		free(selector);
		return NULL;
	}

	struct wakers *wakers = wakers_init(epoll_size);
	if (!wakers) {
		close(event);
		close(epfd);
		free(selector);
		return NULL;
	}

	struct io_queue *queue = io_queue_init();
	if (!queue) {
		wakers_free(wakers);
		close(event);
		close(epfd);
		free(selector);
		return NULL;
	}

	selector->epfd = epfd;
	selector->event = event;
	selector->wakers = wakers;
	selector->queue = queue;

	return selector;
}

void io_selector_free(struct io_selector *selector)
{
	assert(selector != NULL);

	wakers_clean(selector->wakers);

	while (!io_queue_is_empty(selector->queue)) {
		struct io_ops *ops = io_queue_recv(selector->queue);

		if (ops->type == IO_OPS_REMOVE) {
			if (ops->fd != 0) {
				printf("Clean (%d)\n", ops->fd);
				shutdown(ops->fd, SHUT_RDWR);
				close(ops->fd);
			}
		}

		if (ops->waker.ptr) {
			ops->waker.free(ops->waker.ptr);
		}

		free(ops);
	}

	wakers_free(selector->wakers);
	io_queue_free(selector->queue);
	close(selector->event);
	close(selector->epfd);
	free(selector);
}

int io_selector_spawn(struct io_selector *selector,
		      pthread_t *__restrict __newthread)
{
	assert(selector != NULL);

	if (pthread_create(__newthread, NULL, io_selector_select,
			   (void *)selector) != 0) {
		perror("io_selector_run: pthread_create failed to create io_selector "
		       "thread");
		return -1;
	}

	return 0;
}

void io_selector_add_event(struct io_selector *selector, uint32_t flags, int fd,
			   struct waker waker, struct wakers *wakers)
{
	assert(selector != NULL);
	assert(wakers != NULL);

	struct epoll_event ev;

	ev.events = flags | EPOLLONESHOT;
	ev.data.fd = fd;

	errno = 0;

	if (epoll_ctl(selector->epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		// perror("io_selector_add_event: epoll_ctl failed to add event to epoll instance");

		if (errno == EEXIST) { // already exists
			if (epoll_ctl(selector->epfd, EPOLL_CTL_MOD, fd, &ev) ==
			    -1) {
				perror("io_selector_add_event: epoll_ctl failed to modify "
				       "event in epoll instance");
				exit(EXIT_FAILURE);
			}
		} else {
			perror("io_selector_add_event: epoll_ctl failed to add event to "
			       "epoll instance.");
			exit(EXIT_FAILURE);
		}
	}

	if (!wakers_find(wakers, fd)) {
		if (wakers_insert(wakers, fd, waker) < 1) {
			waker.free(waker.ptr);
		}
	}
}

void io_selector_remove_event(struct io_selector *selector, int fd,
			      struct wakers *wakers)
{
	assert(selector != NULL);
	assert(wakers != NULL);

	struct epoll_event ev;

	ev.events = 0;
	ev.data.fd = fd;

	if (epoll_ctl(selector->epfd, EPOLL_CTL_DEL, fd, &ev) == -1) {
		perror("io_selector_remove_event: epoll_ctl failed to remove event "
		       "from epoll instance");
		exit(EXIT_FAILURE);
	}

	struct waker waker = wakers_remove(wakers, fd);
	if (waker.ptr) {
		waker.free(waker.ptr);
	}

	printf("Close (%d)\n", fd);

	shutdown(fd, SHUT_RDWR);
	close(fd);
}

void *io_selector_select(void *arg)
{
	assert(arg != NULL);

	struct io_selector *selector = (struct io_selector *)arg;

	struct epoll_event ev = {
		0,
	};
	ev.events = EPOLLIN;
	ev.data.fd = selector->event;

	if (epoll_ctl(selector->epfd, EPOLL_CTL_ADD, selector->event, &ev) ==
	    -1) {
		perror("io_selector_select: epoll_ctl failed to add event to epoll "
		       "instance!");
		exit(EXIT_FAILURE);
	}

	struct epoll_event *events = (struct epoll_event *)malloc(
		selector->wakers->capacity * sizeof(struct epoll_event));
	if (!events) {
		perror("io_selector_select: malloc failed to allocate events");
		exit(EXIT_FAILURE);
	}

	while (running) {
		int n = epoll_wait(selector->epfd, events,
				   selector->wakers->capacity, 10);

		if (n == 0) {
			continue;
		}

		if (n == -1) {
			if (errno == EINTR) {
				continue;
			}

			free(events);
			perror("io_selector_select: epoll_wait failed to wait for events");
			exit(EXIT_FAILURE);
		}

		uint64_t val;
		ssize_t ret;

		for (int i = 0; i < n; i++) {
			if (events[i].data.fd == selector->event) {
				errno = 0;
				ret = read(selector->event, &val, sizeof(val));
				if (ret < 0) {
					if (errno == EWOULDBLOCK ||
					    errno == EAGAIN || errno == EINTR) {
						continue;
					} else {
						free(events);
						perror("io_selector_select: read failed to read eventfd");
						exit(EXIT_FAILURE);
					}
				}

				while (!io_queue_is_empty(selector->queue)) {
					struct io_ops *ops =
						io_queue_recv(selector->queue);

					if (ops->type == IO_OPS_ADD) {
						io_selector_add_event(
							selector, ops->flags,
							ops->fd, ops->waker,
							selector->wakers);
					} else if (ops->type == IO_OPS_REMOVE) {
						io_selector_remove_event(
							selector, ops->fd,
							selector->wakers);
					}
					free(ops);
				}
			} else {
				struct waker waker = wakers_remove(
					selector->wakers, events[i].data.fd);
				task_wake(waker.ptr);
			}
		}
	}

	free(events);
	return NULL;
}

void io_selector_register(struct io_selector *selector, uint32_t flags, int fd,
			  struct waker waker)
{
	assert(selector != NULL);

	struct io_ops *ops = (struct io_ops *)malloc(sizeof(struct io_ops));
	if (!ops) {
		perror("io_selector_register: malloc failed to allocate io_ops");
		exit(EXIT_FAILURE);
	}

	ops->type = IO_OPS_ADD;
	ops->flags = flags;
	ops->fd = fd;
	ops->waker = waker;

	io_queue_send(selector->queue, ops);
	eventfd_write(selector->event, 1);
}

void io_selector_unregister(struct io_selector *selector, int fd)
{
	assert(selector != NULL);

	struct io_ops *ops = (struct io_ops *)malloc(sizeof(struct io_ops));
	if (!ops) {
		perror("io_selector_unregister: malloc failed to allocate io_ops");
		return;
	}

	ops->type = IO_OPS_REMOVE;
	ops->flags = 0;
	ops->fd = fd;
	ops->waker = (struct waker){ .ptr = NULL, .wake = NULL, .free = NULL };

	io_queue_send(selector->queue, ops);
	eventfd_write(selector->event, 1);
}
