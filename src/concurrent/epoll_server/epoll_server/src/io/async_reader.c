#include "../future.h"
#include "../futures/readline.h"
#include "../scheduler/io_selector.h"
#include "async_reader.h"

#include <sys/epoll.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

struct async_reader *async_reader_init(struct io_selector *selector, int cfd)
{
	if (!selector) {
		perror("async_reader_init: selector is NULL");
		exit(EXIT_FAILURE);
	}

	struct async_reader *r =
		(struct async_reader *)malloc(sizeof(struct async_reader));

	if (!r) {
		perror("async_reader_init: malloc failed to allocate async_reader");
		exit(EXIT_FAILURE);
	}

	int flags = fcntl(cfd, F_GETFL, 0);
	if (flags < 0) {
		perror("async_reader_init: fcntl failed to get flags");
		exit(EXIT_FAILURE);
	}

	if (fcntl(cfd, F_SETFL, flags | O_NONBLOCK) < 0) {
		perror("async_reader_init: fcntl failed to set flags");
		exit(EXIT_FAILURE);
	}

	r->cfd = cfd;
	r->selector = selector;

	return r;
}

void async_reader_free(struct async_reader *reader)
{
	if (!reader) {
		perror("async_reader_free: async_reader is NULL");
		return;
	}

	// move cfd to io_selector
	io_selector_unregister(reader->selector, reader->cfd);
	free(reader);
}

struct future *async_reader_readline(struct async_reader *reader)
{
	if (!reader) {
		perror("async_reader_readline: reader is NULL");
		exit(EXIT_FAILURE);
	}

	return readline_init(reader->selector, reader->cfd);
}
