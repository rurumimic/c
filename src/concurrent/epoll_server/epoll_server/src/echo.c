#include "echo.h"
#include "async_listener.h"
#include "async_reader.h"
#include "future.h"
#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct future *echo_init(struct async_listener *listener, struct future *accept,
			 struct async_reader *reader, int cfd)
{
	struct future *f = (struct future *)malloc(sizeof(struct future));

	if (!f) {
		perror("echo_init: malloc failed to allocate echo");
		exit(EXIT_FAILURE);
	}

	struct echo_data *data =
		(struct echo_data *)malloc(sizeof(struct echo_data));
	data->listener = listener;
	data->reader = reader;
	data->cfd = cfd;
	data->accept = accept;
	data->readline = NULL; // init in echo_poll - async_reader_readline

	f->state = FUTURE_INIT;
	f->poll = echo_poll;
	f->data = data;

	// printf("echo_init: f: %p, sfd: %d, cfd: %d, poll: %p\n", f, listener->sfd, cfd, f->poll);

	return f;
}

enum poll_state echo_poll(struct future *f, struct channel *c)
{
	printf("echo_poll\n");

	struct echo_data *data = (struct echo_data *)f->data;
	struct async_listener *listener = data->listener;
	struct async_reader *reader = data->reader;

	if (f->state == FUTURE_INIT) {
		data->readline = async_reader_readline(f, reader);
		f->state = FUTURE_RUNNING;
	}

	while (running) {
		enum poll_state readline_state = data->readline->poll(data->readline, c);

		if (readline_state == POLL_PENDING) {
			return POLL_PENDING;
		}

		struct readline_data *result = (struct readline_data *)data->readline->data;

		if (result->len <= 0) {
      break;
		}

		printf("read (%d): %s\n", data->cfd, result->lines);
		write(data->cfd, result->lines, result->len);
		fsync(data->cfd);
	}

  printf("close: %d\n", data->cfd);
  echo_data_free(data);

  return POLL_READY;
}

void echo_data_free(struct echo_data *data)
{
	free(data->readline->data);
	free(data->readline);
	async_reader_free(data->reader);
	async_listener_accept_free(data->accept);
	close(data->cfd);
	free(data);
}
