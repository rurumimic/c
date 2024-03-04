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

struct future *server_init(int port, struct io_selector *selector,
			   struct spawner *spawner)
{
  if (!selector) {
    perror("server_init: selector is NULL");
    exit(EXIT_FAILURE);
  }

  if (!spawner) {
    perror("server_init: spawner is NULL");
    exit(EXIT_FAILURE);
  }

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

	if (listen(listener->sfd, 10) < 0) {
		perror("server: listen failed");
		exit(EXIT_FAILURE);
	}

	data->listener = listener;
	data->spawner = spawner;
	data->selector = selector;

	f->poll = server_poll;
	f->data = data;
	f->free = server_free;

	return f;
}

enum poll_state server_poll(struct future *f, struct channel *c)
{
  if (!f) {
    perror("server_poll: future is NULL");
    exit(EXIT_FAILURE);
  }

  if (!c) {
    perror("server_poll: channel is NULL");
    exit(EXIT_FAILURE);
  }

	struct server_data *data = (struct server_data *)f->data;
	struct io_selector *selector = data->selector;
	struct spawner *spawner = data->spawner;
	struct async_listener *listener = data->listener;

	while (running) {
		struct future *accept = async_listener_accept(f, listener);

		enum poll_state accept_state = accept->poll(accept, c);
		if (accept_state == POLL_PENDING) {
			return POLL_PENDING;
		}

		struct accept_data *result = (struct accept_data *)accept->data;
		int cfd = result->cfd;
    printf("Accept (%d): %s\n", cfd, result->cip);
		async_listener_accept_free(accept);

		// move cfd to reader
		// move reader to echo
		spawner_spawn(spawner,
			      echo_init(async_reader_init(selector, cfd)));
	}

	return POLL_READY;
}

void server_free(struct future *f)
{
	if (!f) {
		perror("server_free: future is NULL");
		return;
	}

	struct server_data *data = (struct server_data *)f->data;
	if (data) {
		async_listener_free(data->listener);
		free(data);
	} else {
		perror("server_free: data is NULL");
	}

	free(f);
}
