#include "../future.h"
#include "../futures/readline.h"
#include "../scheduler/io_selector.h"
#include "async_reader.h"

#include <assert.h>
#include <error.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

struct async_reader *async_reader_init(struct io_selector *selector, int cfd)
{
	assert(selector != NULL);

	struct async_reader *reader =
		(struct async_reader *)malloc(sizeof(struct async_reader));

	if (!reader) {
		perror("async_reader_init: malloc failed to allocate async_reader");
		return NULL;
	}

	int flags = fcntl(cfd, F_GETFL, 0);
	if (flags < 0) {
		perror("async_reader_init: fcntl failed to get flags");
		shutdown(cfd, SHUT_RDWR);
		close(cfd);
		free(reader);
		return NULL;
	}

	if (fcntl(cfd, F_SETFL, flags | O_NONBLOCK) < 0) {
		perror("async_reader_init: fcntl failed to set flags");
		shutdown(cfd, SHUT_RDWR);
		close(cfd);
		free(reader);
		return NULL;
	}

	reader->cfd = cfd;
	reader->selector = selector;

	return reader;
}

void async_reader_free(struct async_reader *reader)
{
	assert(reader != NULL);

	// move cfd to io_selector
	io_selector_unregister(reader->selector, reader->cfd);
	free(reader);
}

struct future *async_reader_readline(struct async_reader *reader)
{
	assert(reader != NULL);

	return readline_init(reader->selector, reader->cfd);
}
