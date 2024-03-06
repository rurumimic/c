#ifndef _READLINE_H
#define _READLINE_H

#include <stddef.h>

#define MAX_CHARS 1024

#include "../future.h"
#include "../channel.h"
#include "../io_selector.h"

struct readline_data {
	// ref
	struct io_selector *selector;
	struct future *echo;
	int cfd;

	// owned
	char lines[1024];
	size_t len;
};

struct future *readline_init(struct future *echo, struct io_selector *selector,
			     int cfd);
void readline_free(struct future *f);

enum poll_state readline_poll(struct future *f, struct channel *c);

#endif // !_READLINE_H
