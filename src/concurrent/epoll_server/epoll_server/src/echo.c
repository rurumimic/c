#include "echo.h"
#include "async_listener.h"
#include "async_reader.h"
#include "future.h"
#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct future *echo_init(struct async_reader *reader)
{
  if (!reader) {
    perror("echo_init: reader is NULL");
    exit(EXIT_FAILURE);
  }

	struct future *f = (struct future *)malloc(sizeof(struct future));

	if (!f) {
		perror("echo_init: malloc failed to allocate echo");
		exit(EXIT_FAILURE);
	}

	struct echo_data *data =
		(struct echo_data *)malloc(sizeof(struct echo_data));

	data->reader = reader;

	f->poll = echo_poll;
	f->data = data;
  f->free = echo_free;

	return f;
}

enum poll_state echo_poll(struct future *f, struct channel *c)
{
  if (!f) {
    perror("echo_poll: future is NULL");
    exit(EXIT_FAILURE);
  }

  if (!c) {
    perror("echo_poll: channel is NULL");
    exit(EXIT_FAILURE);
  }

	struct echo_data *data = (struct echo_data *)f->data;
  if (!data) {
    perror("echo_poll: data is NULL");
    exit(EXIT_FAILURE);
  }

	struct async_reader *reader = data->reader;
	int cfd = reader->cfd;

	struct io_selector *selector = reader->selector;

	while (running) {
		struct future *readline =
			async_reader_readline(f, selector, cfd);

		enum poll_state readline_state = readline->poll(readline, c);
		if (readline_state == POLL_PENDING) {
			return POLL_PENDING;
		}

		struct readline_data *result =
			(struct readline_data *)readline->data;

    if (!result) {
      perror("echo_poll: result is NULL");
      exit(EXIT_FAILURE);
    }

		if (result->len <= 0) {
			async_reader_readline_free(readline);
			break;
		}

		write(cfd, result->lines, result->len);
		fsync(cfd);

    printf("Echo (%d): %s", cfd, result->lines);
		async_reader_readline_free(readline);
	}

  printf("Close (%d)\n", cfd);
  
	return POLL_READY;
}

void echo_free(struct future *f)
{
	if (!f) {
		perror("echo_free: future is NULL");
		return;
	}

	struct echo_data *data = (struct echo_data *)f->data;
	if (data) {
		async_reader_free(data->reader);
		free(data);
	} else {
		perror("echo_free: data is NULL");
	}

	free(f);
}
