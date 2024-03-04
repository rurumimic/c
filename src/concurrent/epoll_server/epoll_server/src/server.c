#include "server.h"
#include "async_listener.h"
#include "future.h"
#include "spawner.h"

#include "async_reader.h"
#include "echo.h"

#include "global.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

struct future *server_init(int port, struct io_selector *selector, struct spawner *spawner) {
    struct future *f = (struct future *)malloc(sizeof(struct future));

    if (!f) {
        perror("server_init: malloc failed to allocate server");
        exit(EXIT_FAILURE);
    }

    struct server_data *data = (struct server_data *)malloc(sizeof(struct server_data));

    if (!data) {
        perror("server_init: malloc failed to allocate server_data");
        exit(EXIT_FAILURE);
    }

    /* server init */
    struct async_listener *listener = async_listener_init(port, selector);

    if (listen(listener->sfd, 10) < 0) {
        perror("server: listen failed");
        exit(EXIT_FAILURE);
    }

    /* server data */
    data->listener = listener;
    data->spawner = spawner;
    data->accept = NULL;

    f->state = FUTURE_INIT;
    f->poll = server_poll;
    f->data = data;

    return f;
}

enum poll_state server_poll(struct future *f, struct channel *c) {
    struct server_data *data = (struct server_data *)f->data;
    struct spawner *spawner = data->spawner;
    struct async_listener *listener = data->listener;

    while (running) {
      data->accept = async_listener_accept(f, listener); 
      // printf("server_poll: accept before poll state: %d \n", data->accept->state);
      // printf("server_poll -> accept_poll\n");
      enum poll_state accept_state = data->accept->poll(data->accept, c);

      if (accept_state == POLL_PENDING) {
        return POLL_PENDING;
      }
      // printf("server_poll: accept after poll state: %d \n", data->accept->state);

      struct accept_data *result = (struct accept_data *)data->accept->data;

      struct async_reader *reader = result->reader;
      int cfd = result->cfd;

      struct future *echo = echo_init(listener, data->accept, reader, cfd);
      printf("server_poll: echo_init (listener, reader, cfd: %d)\n", cfd);
      spawner_spawn(spawner, echo); // readline
  }

  // free server;
  return POLL_READY;
}

void server_data_free(struct server_data *data) {
    if (!data) {
        perror("server_data_free: server_data is NULL");
        return;
    }

    async_listener_free(data->listener);
    spawner_free(data->spawner);
    free(data);
}
