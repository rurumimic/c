#include "shutdown.h"
#include "../io_selector.h"

#include <stdio.h>
#include <stdlib.h>

struct future *shutdown_init(struct io_selector *selector)
{
	struct future *f = (struct future *)malloc(sizeof(struct future));

	if (!f) {
		perror("shutdown_init: malloc failed to allocate future");
		exit(EXIT_FAILURE);
	}

	struct shutdown_data *data =
		(struct shutdown_data *)malloc(sizeof(struct shutdown_data));

	if (!data) {
		perror("shutdown_init: malloc failed to allocate shutdown_data");
		exit(EXIT_FAILURE);
	}

	data->selector = selector;

	f->poll = shutdown_poll;
	f->data = data;
	f->free = shutdown_free;

	return f;
}

void shutdown_free(struct future *f)
{
	if (!f) {
		perror("shutdown_free: future is NULL");
		return;
	}

	struct shutdown_data *data = (struct shutdown_data *)f->data;
	if (data) {
		free(data);
	} else {
		perror("shutdown_free: data is NULL");
	}

	free(f);
}

enum poll_state shutdown_poll(struct future *f, struct channel *c)
{
	if (!f) {
		perror("shutdown_poll: future is NULL");
		exit(EXIT_FAILURE);
	}

	if (!c) {
		perror("shutdown_poll: channel is NULL");
		exit(EXIT_FAILURE);
	}

	struct shutdown_data *data = (struct shutdown_data *)f->data;
	struct io_selector *selector = data->selector;

	io_selector_shutdown(selector);

	return POLL_READY;
}
