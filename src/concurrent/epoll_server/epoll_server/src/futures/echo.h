#ifndef _ECHO_H
#define _ECHO_H

#include "../future.h"

enum async_echo_state {
	ECHO_READING,
};

struct echo_data {
	// owned
	enum async_echo_state state;
	struct async_reader *reader;

	// ref
};

struct future *echo_init(struct async_reader *reader);
void echo_free(struct future *f);

struct poll echo_poll(struct future *f, struct context cx);

#endif // _ECHO_H
