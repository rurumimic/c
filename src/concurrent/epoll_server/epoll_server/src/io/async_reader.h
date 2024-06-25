#ifndef _ASYNC_READER_H
#define _ASYNC_READER_H

#include <stddef.h>

#include "../future.h"
#include "../scheduler/io_selector.h"

#define MAX_CHARS 1024

struct async_reader {
	// ref
	struct io_selector *selector;
	int cfd;
};

struct async_reader *async_reader_init(struct io_selector *selector, int cfd);
void async_reader_free(struct async_reader *reader);
struct future *async_reader_readline(struct async_reader *reader);

#endif // !_ASYNC_READER_H
