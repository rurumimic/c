#include "server.h"
#include "async_listener.h"
#include "future.h"
#include "spawner.h"

#include "async_reader.h"
#include "echo.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

struct future *server_init(int port, struct io_selector *selector,
                           struct spawner *spawner) {
    struct future *f = (struct future *)malloc(sizeof(struct future));

    if (!f) {
        perror("server_init: malloc failed to allocate server");
        exit(EXIT_FAILURE);
    }

    struct server_data *data =
        (struct server_data *)malloc(sizeof(struct server_data));

    if (!data) {
        perror("server_init: malloc failed to allocate server_data");
        exit(EXIT_FAILURE);
    }

    /* server init */
    struct async_listener *listener = async_listener_init(port, selector);

    if (listen(listener->fd, 10) < 0) {
        perror("server: listen failed");
        exit(EXIT_FAILURE);
    }

    /* server data */
    data->listener = listener;
    data->spawner = spawner;
    data->accept_future = NULL;

    f->state = FUTURE_INIT;
    f->poll = server_poll;
    f->data = data;

    return f;
}

enum poll_state server_poll(struct future *f, struct channel *c) {
    // list variables:
    struct server_data *data = (struct server_data *)f->data;
    struct async_listener *listener = data->listener;
    struct spawner *spawner = data->spawner;
    struct future *accept_future = data->accept_future;

    if (accept_future == NULL) {
      data->accept_future = async_listener_accept(listener); 
      struct task *t = task_init(data->accept_future, c);
      channel_send(c, sizeof(struct task), t, task_free);

      return POLL_PENDING;
    }

    if (accept_future->state == FUTURE_INIT) {
      return POLL_PENDING;
    }

    if (accept_future->state == FUTURE_STOPPED) {
    	async_listener_accept_free(accept_future);
	data->accept_future = NULL;
    	return POLL_PENDING;
    }

    printf("server running...\n");

    accept_future->state = FUTURE_STOPPED;
    struct accept_data *result = (struct accept_data *)accept_future->data; 

    struct async_reader *reader = result->reader;
    int fd = result->fd;
    // addr = accept_result.addr

//    async_listener_accept_free(accept_future);
//    data->accept_future = NULL;

    spawner_spawn(spawner, echo_init(listener, reader, fd)); // readline
    // queue read
    return POLL_PENDING;
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
