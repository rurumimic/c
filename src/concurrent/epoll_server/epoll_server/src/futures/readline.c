#include "../future.h"
#include "../scheduler/channel.h"
#include "../scheduler/io_selector.h"
#include "readline.h"

#include <error.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

struct future *readline_init(struct io_selector *selector, int cfd)
{
	if (!selector) {
		perror("readline: selector is NULL");
		exit(EXIT_FAILURE);
	}

	struct future *f = (struct future *)malloc(sizeof(struct future));

	if (!f) {
		perror("readline: malloc failed to allocate future");
		exit(EXIT_FAILURE);
	}

	struct readline_data *data =
		(struct readline_data *)malloc(sizeof(struct readline_data));

	if (!data) {
		perror("readline: malloc failed to allocate readline_data");
		exit(EXIT_FAILURE);
	}

	data->selector = selector;
	data->cfd = cfd;
	memset(data->lines, 0, MAX_CHARS);

	f->data = data;
	f->poll = readline_poll;
	f->free = readline_free;

	return f;
}

void readline_free(struct future *f)
{
	if (!f) {
		perror("readline_free: future is NULL");
		return;
	}

	struct readline_data *data = (struct readline_data *)f->data;

	if (data) {
		free(data);
	} else {
		perror("readline_free: data is NULL");
	}

	free(f);
}

struct poll readline_poll(struct future *f, struct context cx)
{
	if (!f) {
		perror("readline_poll: future is NULL");
		exit(EXIT_FAILURE);
	}

	struct readline_data *data = (struct readline_data *)f->data;
	if (!data) {
		perror("readline_poll: data is NULL");
		exit(EXIT_FAILURE);
	}

	struct io_selector *selector = data->selector;
	int cfd = data->cfd;

	errno = 0;
	char buf[MAX_CHARS] = {
		0,
	};
	ssize_t n = read(cfd, buf, MAX_CHARS - 1);

	if (n < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
			io_selector_register(selector, EPOLLIN, cfd, cx.waker);
			return (struct poll){ .state = POLL_PENDING,
					      .output = NULL,
					      .free = NULL };
		} else {
			data->len = -1;
			perror("readline_poll: read failed");
			return (struct poll){ .state = POLL_READY,
					      .output = data,
					      .free = NULL };
		}
	}

	if (n == 0) {
		data->len = 0;
		return (struct poll){ .state = POLL_READY,
				      .output = data,
				      .free = NULL };
	}

	data->len = n;
	memcpy(data->lines, buf, MAX_CHARS);

	return (struct poll){ .state = POLL_READY,
			      .output = data,
			      .free = NULL };
}
