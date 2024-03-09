#include "io_selector.h"
#include "io_queue.h"
#include "task.h"
#include "wakers.h"
#include "global.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>

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

	int event = eventfd(0, 0);

	if (event == -1) {
		perror("io_selector_init: eventfd failed to create eventfd instance");
		exit(EXIT_FAILURE);
	}

	s->event = event;
	s->wakers = wakers_init(epoll_size);
	s->queue = io_queue_init();

	pthread_mutex_init(&s->wakers_mutex, NULL);
	pthread_mutex_init(&s->queue_mutex, NULL);

	return s;
}

void io_selector_free(struct io_selector *s)
{
	if (!s) {
		perror("io_selector_free: io_selector is NULL");
		return;
	}

	close(s->epfd);
	close(s->event);
	wakers_free(s->wakers);
	io_queue_free(s->queue);
	free(s);
}

pthread_t io_selector_spawn(struct io_selector *s)
{
	if (!s) {
		perror("io_selector_spawn: io_selector is NULL");
		exit(EXIT_FAILURE);
	}

	pthread_t tid;

	if (pthread_create(&tid, NULL, io_selector_select, (void *)s) != 0) {
		perror("io_selector_run: pthread_create failed to create io_selector "
		       "thread");
		exit(EXIT_FAILURE);
	}

	return tid;
}

void io_selector_add_event(struct io_selector *s, uint32_t flags, int fd,
			   struct task *task, struct wakers *wakers)
{
	if (!s) {
		perror("io_selector_add_event: io_selector is NULL");
		exit(EXIT_FAILURE);
	}

	if (!task) {
		perror("io_selector_add_event: task is NULL");
		exit(EXIT_FAILURE);
	}

	if (!wakers) {
		perror("io_selector_add_event: wakers is NULL");
		exit(EXIT_FAILURE);
	}

	struct epoll_event ev;

	ev.events = flags | EPOLLONESHOT;
	ev.data.fd = fd;

	errno = 0;

	if (epoll_ctl(s->epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		// perror("io_selector_add_event: epoll_ctl failed to add event to epoll instance");

		if (errno == EEXIST) { // already exists
			if (epoll_ctl(s->epfd, EPOLL_CTL_MOD, fd, &ev) == -1) {
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
		wakers_insert(wakers, fd, task);
	}
}

void io_selector_remove_event(struct io_selector *s, int fd,
			      struct wakers *wakers)
{
	if (!s) {
		perror("io_selector_remove_event: io_selector is NULL");
		exit(EXIT_FAILURE);
	}

	if (!wakers) {
		perror("io_selector_remove_event: wakers is NULL");
		exit(EXIT_FAILURE);
	}

	struct epoll_event ev;

	ev.events = 0;
	ev.data.fd = fd;

	if (epoll_ctl(s->epfd, EPOLL_CTL_DEL, fd, &ev) == -1) {
		perror("io_selector_remove_event: epoll_ctl failed to remove event "
		       "from epoll instance");
		exit(EXIT_FAILURE);
	}

	struct task *task = wakers_remove(wakers, fd);

	if (task) {
		task_free(task);
	}

	shutdown(fd, SHUT_RDWR);
	close(fd);
}

void *io_selector_select(void *arg)
{
	if (!arg) {
		perror("io_selector_select: arg is NULL");
		exit(EXIT_FAILURE);
	}

	struct io_selector *s = (struct io_selector *)arg;

	struct epoll_event ev = {
		0,
	};
	ev.events = EPOLLIN;
	ev.data.fd = s->event;

	if (epoll_ctl(s->epfd, EPOLL_CTL_ADD, s->event, &ev) == -1) {
		perror("io_selector_select: epoll_ctl failed to add event to epoll "
		       "instance!");
		exit(EXIT_FAILURE);
	}

	struct epoll_event *events = (struct epoll_event *)malloc(
		s->wakers->capacity * sizeof(struct epoll_event));
	if (!events) {
		perror("io_selector_select: malloc failed to allocate events");
		exit(EXIT_FAILURE);
	}

	while (running) {
		int n = epoll_wait(s->epfd, events, s->wakers->capacity, 10);

		if (n == 0) {
			continue;
		}

		if (n == -1) {
			if (errno == EINTR) {
				continue;
			}
			perror("io_selector_select: epoll_wait failed to wait for events");
			exit(EXIT_FAILURE);
		}

		uint64_t val;
		ssize_t ret;

		pthread_mutex_lock(&s->wakers_mutex);
		for (int i = 0; i < n; i++) {
			if (events[i].data.fd == s->event) {
				errno = 0;
				ret = read(s->event, &val, sizeof(val));
				if (ret < 0) {
					if (errno == EWOULDBLOCK ||
					    errno == EAGAIN || errno == EINTR) {
						continue;
					} else {
						perror("io_selector_select: read failed to read eventfd");
						exit(EXIT_FAILURE);
					}
				}

				pthread_mutex_lock(&s->queue_mutex);
				while (!io_queue_is_empty(s->queue)) {
					struct io_ops *ops =
						io_queue_recv(s->queue);

					if (ops->type == IO_OPS_ADD) {
						io_selector_add_event(
							s, ops->flags, ops->fd,
							ops->task, s->wakers);
					} else if (ops->type == IO_OPS_REMOVE) {
						io_selector_remove_event(
							s, ops->fd, s->wakers);
					}
					free(ops);
				}
				pthread_mutex_unlock(&s->queue_mutex);
			} else {
				struct task *task = wakers_remove(
					s->wakers, events[i].data.fd);
				task_wake_by_ref(task);
			}
		}
		pthread_mutex_unlock(&s->wakers_mutex);
	}

	free(events);

	return NULL;
}

void io_selector_register(struct io_selector *s, uint32_t flags, int fd,
			  struct task *task)
{
	if (!s) {
		perror("io_selector_register: io_selector is NULL");
		exit(EXIT_FAILURE);
	}

	if (!task) {
		perror("io_selector_register: task is NULL");
		exit(EXIT_FAILURE);
	}

	pthread_mutex_lock(&s->queue_mutex);

	struct io_ops *ops = (struct io_ops *)malloc(sizeof(struct io_ops));
	if (!ops) {
		perror("io_selector_register: malloc failed to allocate io_ops");
		exit(EXIT_FAILURE);
	}

	ops->type = IO_OPS_ADD;
	ops->flags = flags;
	ops->fd = fd;
	ops->task = task;

	io_queue_send(s->queue, ops);

	eventfd_write(s->event, 1);

	pthread_mutex_unlock(&s->queue_mutex);
}

void io_selector_unregister(struct io_selector *s, int fd)
{
	if (!s) {
		perror("io_selector_unregister: io_selector is NULL");
		exit(EXIT_FAILURE);
	}

	pthread_mutex_lock(&s->queue_mutex);

	struct io_ops *ops = (struct io_ops *)malloc(sizeof(struct io_ops));
	if (!ops) {
		perror("io_selector_unregister: malloc failed to allocate io_ops");
		exit(EXIT_FAILURE);
	}

	ops->type = IO_OPS_REMOVE;
	ops->flags = 0;
	ops->fd = fd;
	ops->task = NULL;

	io_queue_send(s->queue, ops);

	eventfd_write(s->event, 1);

	pthread_mutex_unlock(&s->queue_mutex);
}

