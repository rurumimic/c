#ifndef _READLINE_H
#define _READLINE_H

#include <stddef.h>

#define MAX_CHARS 1024

#include "../future.h"
#include "../scheduler/channel.h"
#include "../scheduler/io_selector.h"

struct readline_data {
	// ref
	struct io_selector *selector;
	int cfd;

	// owned
	char lines[1024];
	size_t len;
};

struct future *readline_init(struct io_selector *selector, int cfd);
void readline_free(struct future *f);
struct poll readline_poll(struct future *f, struct context cx);

#endif // !_READLINE_H
