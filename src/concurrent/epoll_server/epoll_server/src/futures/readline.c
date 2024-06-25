#include "../future.h"
#include "../scheduler/channel.h"
#include "../scheduler/io_selector.h"
#include "readline.h"

#include <assert.h>
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
	assert(selector != NULL);

	struct future *future = (struct future *)malloc(sizeof(struct future));

	if (!future) {
		perror("readline: malloc failed to allocate future");
		return NULL;
	}

	struct readline_data *data =
		(struct readline_data *)malloc(sizeof(struct readline_data));

	if (!data) {
		perror("readline: malloc failed to allocate readline_data");
		free(future);
		return NULL;
	}

	data->selector = selector;
	data->cfd = cfd;
	memset(data->lines, 0, MAX_CHARS);

	future->data = data;
	future->poll = readline_poll;
	future->free = readline_free;

	return future;
}

void readline_free(struct future *future)
{
	assert(future != NULL);

	struct readline_data *data = (struct readline_data *)future->data;

	if (data) {
		free(data);
	} else {
		perror("readline_free: data is NULL");
	}

	free(future);
}

struct poll readline_poll(struct future *future, struct context context)
{
	assert(future != NULL);

	struct readline_data *data = (struct readline_data *)future->data;
	if (!data) {
		perror("readline_poll: data is NULL");
		return (struct poll){ .state = POLL_READY,
				      .output = NULL,
				      .free = NULL };
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
			io_selector_register(selector, EPOLLIN, cfd,
					     context.waker);
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
