#include "../future.h"
#include "../global.h"
#include "../io/async_listener.h"
#include "../scheduler/spawner.h"
#include "accept.h"
#include "../io/async_reader.h"
#include "echo.h"
#include "server.h"

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
		perror("server_init: malloc failed to allocate data");
		exit(EXIT_FAILURE);
	}

	struct async_listener *listener = async_listener_init(port, selector);

	if (listen(listener->sfd, 10) < 0) {
		perror("server: listen failed");
		exit(EXIT_FAILURE);
	}

	data->state = SERVER_LISTENING;
	data->selector = selector;
	data->spawner = spawner;
	data->listener = listener;

	f->data = data;
	f->poll = server_poll;
	f->free = server_free;

	return f;
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

struct poll server_poll(struct future *f, struct context cx)
{
	if (!f) {
		perror("server_poll: future is NULL");
		exit(EXIT_FAILURE);
	}

	struct server_data *server = (struct server_data *)f->data;
	struct io_selector *selector = server->selector;
	struct spawner *spawner = server->spawner;
	struct async_listener *listener = server->listener;

	while (running) {
		if (server->state == SERVER_LISTENING) {
			struct future *accept = async_listener_accept(listener);
			struct poll poll = accept->poll(accept, cx);
			if (poll.state == POLL_PENDING) {
				return (struct poll){ .state = POLL_PENDING,
						      .output = NULL,
						      .free = NULL };
			}

			struct accept_data *result =
				(struct accept_data *)poll.output;
			int cfd = result->cfd;
			printf("Accept (%d): %s\n", cfd, result->cip);
			accept->free(accept);

			// move cfd to reader
			// move reader to echo
			spawner_spawn(spawner, echo_init(async_reader_init(
						       selector, cfd)));
		}
	}

	return (struct poll){ .state = POLL_READY,
			      .output = NULL,
			      .free = NULL };
}
