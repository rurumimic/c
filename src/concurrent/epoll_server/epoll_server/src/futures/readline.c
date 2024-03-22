#include "readline.h"
#include "../future.h"
#include "../channel.h"
#include "../io_selector.h"

#include <sys/epoll.h>
#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

struct future *readline_init(struct future *echo, struct io_selector *selector,
			     int cfd)
{
	if (!echo) {
		perror("async_reader_readline: echo is NULL");
		exit(EXIT_FAILURE);
	}

	if (!selector) {
		perror("async_reader_readline: selector is NULL");
		exit(EXIT_FAILURE);
	}

	struct future *f = (struct future *)malloc(sizeof(struct future));

	if (!f) {
		perror("async_reader_readline: malloc failed to allocate future");
		exit(EXIT_FAILURE);
	}

	struct readline_data *data =
		(struct readline_data *)malloc(sizeof(struct readline_data));

	if (!data) {
		perror("async_reader_readline: malloc failed to allocate readline_data");
		exit(EXIT_FAILURE);
	}

	data->echo = echo;
	data->selector = selector;
	data->cfd = cfd;
	memset(data->lines, 0, MAX_CHARS);

	f->poll = readline_poll;
	f->data = data;
	f->free = readline_free;

	return f;
}

void readline_free(struct future *f)
{
	if (!f) {
		perror("async_reader_readline_free: future is NULL");
		return;
	}

	struct readline_data *data = (struct readline_data *)f->data;

	if (data) {
		free(data);
	} else {
		perror("async_reader_readline_free: data is NULL");
	}

	free(f);
}

enum poll_state readline_poll(struct future *f, struct channel *c)
{
	if (!f) {
		perror("async_reader_readline_poll: future is NULL");
		exit(EXIT_FAILURE);
	}

	if (!c) {
		perror("async_reader_readline_poll: channel is NULL");
		exit(EXIT_FAILURE);
	}

	struct readline_data *data = (struct readline_data *)f->data;
	if (!data) {
		perror("async_reader_readline_poll: data is NULL");
		exit(EXIT_FAILURE);
	}

	struct io_selector *selector = data->selector;
	struct future *echo = data->echo;
	int cfd = data->cfd;

	errno = 0;
	char buf[MAX_CHARS] = {
		0,
	};
	ssize_t n = read(cfd, buf, MAX_CHARS - 1);

	if (n < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
			io_selector_register(selector, EPOLLIN, cfd, task_init(echo, c));
			return POLL_PENDING;
		} else {
			data->len = -1;
			return POLL_READY; // error
		}
	}

	if (n == 0) {
		data->len = 0;
		return POLL_READY;
	}

	data->len = n;
	memcpy(data->lines, buf, MAX_CHARS);

	return POLL_READY;
}
