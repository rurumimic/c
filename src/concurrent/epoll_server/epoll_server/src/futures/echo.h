#ifndef _ECHO_H
#define _ECHO_H

#include "../future.h"

struct echo_data {
	// owned
	struct async_reader *reader;

	// ref
};

struct future *echo_init(struct async_reader *reader);
void echo_free(struct future *f);

enum poll_state echo_poll(struct future *f, struct channel *c);

#endif // _ECHO_H
