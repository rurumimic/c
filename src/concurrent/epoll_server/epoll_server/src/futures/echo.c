#include "../future.h"
#include "../global.h"
#include "../io/async_reader.h"
#include "echo.h"
#include "readline.h"

#include "../poll.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct future *echo_init(struct async_reader *reader)
{
	assert(reader != NULL);

	struct future *future = (struct future *)malloc(sizeof(struct future));

	if (!future) {
		perror("echo_init: malloc failed to allocate echo");
		return NULL;
	}

	struct echo_data *data =
		(struct echo_data *)malloc(sizeof(struct echo_data));

	if (!data) {
		perror("echo_init: malloc failed to allocate echo_data");
		free(future);
		return NULL;
	}

	data->state = ECHO_READING;
	data->reader = reader;

	future->data = data;
	future->poll = echo_poll;
	future->free = echo_free;

	return future;
}

void echo_free(struct future *future)
{
	assert(future != NULL);

	struct echo_data *data = (struct echo_data *)future->data;
	if (data) {
		async_reader_free(data->reader);
		free(data);
	} else {
		perror("echo_free: data is NULL");
	}

	free(future);
}

struct poll echo_poll(struct future *future, struct context context)
{
	assert(future != NULL);

	struct echo_data *echo = (struct echo_data *)future->data;
	if (!echo) {
		perror("echo_poll: data is NULL");
		echo->state = ECHO_FINISHED;
		return (struct poll){ .state = POLL_READY,
				      .output = NULL,
				      .free = NULL };
	}

	struct async_reader *reader = echo->reader;
	int cfd = reader->cfd;

	struct io_selector *selector = reader->selector;

	while (running) {
		if (echo->state == ECHO_READING) {
			struct future *readline = async_reader_readline(reader);
			if (!readline) {
				perror("echo_poll: async_reader_readline failed to read line");
				break;
			}

			struct poll poll = readline->poll(readline, context);
			if (poll.state == POLL_PENDING) {
				readline->free(readline);
				return (struct poll){ .state = POLL_PENDING,
						      .output = NULL,
						      .free = NULL };
			}

			if (poll.output == NULL) {
				readline->free(readline);
				break;
			}

			struct readline_data *result =
				(struct readline_data *)poll.output;

			size_t len = result->len;

			if (result->len <= 0) {
				readline->free(readline);
				break;
			}

			echo->state = ECHO_WRITING;

			write(cfd, result->lines, result->len);
			fsync(cfd);

			printf("Echo (%d): %s", cfd, result->lines);
			readline->free(readline);

			echo->state = ECHO_READING;
		}
	}

	echo->state = ECHO_FINISHED;
	return (struct poll){ .state = POLL_READY,
			      .output = NULL,
			      .free = NULL };
}
