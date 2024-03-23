#include "echo.h"
#include "readline.h"
#include "../async_reader.h"
#include "../future.h"
#include "../global.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

	struct echo_data *data = (struct echo_data *)malloc(sizeof(struct echo_data));

  data->state = ECHO_READING;
	data->reader = reader;

	f->data = data;
	f->poll = echo_poll;
	f->free = echo_free;

	return f;
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

struct poll echo_poll(struct future *f, struct context cx)
{
	if (!f) {
		perror("echo_poll: future is NULL");
		exit(EXIT_FAILURE);
	}

	struct echo_data *echo = (struct echo_data *)f->data;
	if (!echo) {
		perror("echo_poll: data is NULL");
		exit(EXIT_FAILURE);
	}

	struct async_reader *reader = echo->reader;
	int cfd = reader->cfd;

	struct io_selector *selector = reader->selector;

	while (running) {
    if (echo->state == ECHO_READING) {
      struct future *readline = async_reader_readline(reader);
      struct poll poll = readline->poll(readline, cx);
      if (poll.state == POLL_PENDING) {
        return (struct poll){.state = POLL_PENDING, .output = NULL, .free = NULL};
      }
      
      struct readline_data *result = (struct readline_data *)poll.output;
      size_t len = result->len;

      if (result->len <= 0) {
        readline->free(readline);
        break;
      }

      write(cfd, result->lines, result->len);
      fsync(cfd);

      printf("Echo (%d): %s", cfd, result->lines);
      readline->free(readline);
    }
	}

	printf("Close (%d)\n", cfd);

	return (struct poll){ .state = POLL_READY, .output = NULL, .free = NULL };
}
