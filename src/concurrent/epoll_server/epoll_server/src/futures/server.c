#include "../future.h"
#include "../global.h"
#include "../io/async_listener.h"
#include "../io/async_reader.h"
#include "../scheduler/spawner.h"
#include "accept.h"
#include "echo.h"
#include "server.h"

#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define REQUEST_SIZE 10

struct future *server_init(int port, struct io_selector *selector,
			   struct spawner *spawner)
{
  assert(selector != NULL);
  assert(spawner != NULL);

	struct future *future = (struct future *)malloc(sizeof(struct future));

	if (!future) {
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

	if (listen(listener->sfd, REQUEST_SIZE) < 0) {
		perror("server: listen failed");
		exit(EXIT_FAILURE);
	}

	data->state = SERVER_LISTENING;
	data->selector = selector;
	data->spawner = spawner;
	data->listener = listener;

	future->data = data;
	future->poll = server_poll;
	future->free = server_free;

	return future;
}

void server_free(struct future *future)
{
  assert(future != NULL);

	struct server_data *data = (struct server_data *)future->data;
	if (data) {
		async_listener_free(data->listener);
		free(data);
	} else {
		perror("server_free: data is NULL");
	}

	free(future);
}

struct poll server_poll(struct future *future, struct context context)
{
  assert(future != NULL);

	struct server_data *server = (struct server_data *)future->data;
	struct io_selector *selector = server->selector;
	struct spawner *spawner = server->spawner;
	struct async_listener *listener = server->listener;

	while (running) {
		if (server->state == SERVER_LISTENING) {
			struct future *accept = async_listener_accept(listener);
			struct poll poll = accept->poll(accept, context);
			if (poll.state == POLL_PENDING) {
				return (struct poll){ .state = POLL_PENDING,
						      .output = NULL,
						      .free = NULL };
			}

      server->state = SERVER_ACCEPTED;

			struct accept_data *result =
				(struct accept_data *)poll.output;
			int cfd = result->cfd;
			printf("Accept (%d): %s\n", cfd, result->cip);
			accept->free(accept);

			// move cfd to reader
			// move reader to echo
			spawner_spawn(spawner, echo_init(async_reader_init(
						       selector, cfd)));

      server->state = SERVER_LISTENING;
		}
	}

	return (struct poll){ .state = POLL_READY,
			      .output = NULL,
			      .free = NULL };
}
