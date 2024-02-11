#include "io_selector.h"
#include "wakers.h"
#include "channel.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

struct io_selector *io_selector_init(size_t epoll_size) {

  struct io_selector *s = (struct io_selector *)malloc(sizeof(struct io_selector));

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

pthread_t io_selector_spawn(struct io_selector *s) {
  pthread_t tid;

  if (pthread_create(&tid, NULL, io_selector_select, (void *)s) != 0) {
    perror("io_selector_run: pthread_create failed to create io_selector thread");
    exit(EXIT_FAILURE);
  }

  return tid;
}

void *io_selector_select(void *arg) {
  struct io_selector *s = (struct io_selector *)arg;

  return NULL;
}

