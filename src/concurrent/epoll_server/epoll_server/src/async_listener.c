#include "async_listener.h"
#include "future.h"
#include "io_selector.h"
#include "wakers.h"
#include "channel.h"
#include "async_reader.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

struct async_listener *async_listener_init(int port, struct io_selector *selector) {
  struct async_listener *listener = (struct async_listener *)malloc(sizeof(struct async_listener));
  if (!listener) {
    perror("async_listener_init: malloc failed to allocate async_listener");
    exit(EXIT_FAILURE);
  }

  int sfd = socket(AF_INET, SOCK_STREAM, 0);

  int opt = 1;
  if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("async_listener_init: setsockopt failed to set options");
    exit(EXIT_FAILURE);
  }

  if (sfd < 0) {
    perror("async_listener_init: socket failed to create");
    exit(EXIT_FAILURE);
  }

  int flags = fcntl(sfd, F_GETFL, 0);
  if (flags < 0) {
    perror("async_listener_init: fcntl failed to get flags");
    exit(EXIT_FAILURE);
  }
  if (fcntl(sfd, F_SETFL, flags | O_NONBLOCK) < 0) {
    perror("async_listener_init: fcntl failed to set flags");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in address;
  address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
  address.sin_family = AF_INET; // IPv4
  address.sin_port = htons(port);

  if (bind(sfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("async_listener_init: bind failed");
    exit(EXIT_FAILURE);
  }

  listener->sfd = sfd;
  listener->selector = selector;

  return listener;
}

struct future *async_listener_accept(struct future *server, struct async_listener *listener) {
  struct future *f = (struct future *)malloc(sizeof(struct future));

  if (!f) {
    perror("async_listener_accept: malloc failed to allocate future");
    exit(EXIT_FAILURE);
  }

  struct accept_data *data = (struct accept_data *)malloc(sizeof(struct accept_data));

  if (!data) {
    perror("async_listener_accept: malloc failed to allocate accept_data");
    exit(EXIT_FAILURE);
  }

  data->server = server;
  data->listener = listener;
  data->reader = NULL;
  data->cfd = 0;

  f->state = FUTURE_INIT;
  f->poll = async_listener_accept_poll;
  f->data = data;

  return f;
}

enum poll_state async_listener_accept_poll(struct future *f, struct channel *c) {
  struct accept_data *data = (struct accept_data *)f->data;
  struct async_listener *listener = data->listener;
  struct future *server = data->server;

  struct sockaddr_in client_address;
  size_t client_addr_size = sizeof(client_address);
 
  errno = 0;
  int cfd = accept(listener->sfd, (struct sockaddr *)&client_address, (socklen_t *)&client_addr_size);
  if (cfd < 0) {
    if (errno == EWOULDBLOCK) {
      io_selector_register(listener->selector, EPOLLIN, listener->sfd, task_init(server, c));
      return POLL_PENDING;
    } else {
      perror("accept_poll: accept failed");
      exit(EXIT_FAILURE);
    }
  }

  printf("accept: %d\n", cfd);

  f->state = FUTURE_RUNNING;

  struct async_reader *reader = async_reader_init(cfd, listener->selector);

  data->reader = reader;
  data->cfd = cfd;

  return POLL_READY;
}

void async_listener_free(struct async_listener *listener) {
  io_selector_unregister(listener->selector, listener->sfd);
  close(listener->sfd);
  io_selector_free(listener->selector);
  free(listener);
}

void async_listener_accept_data_free(struct accept_data *data) {
  free(data);
}

void async_listener_accept_free(struct future *accept) {
  if (!accept) {
    perror("async_listener_accept_free: future is NULL");
    return;
  }

  struct accept_data *data = (struct accept_data *)accept->data;
  free(data);
  free(accept);
}
