#ifndef _CHANNEL_H
#define _CHANNEL_H

#include <stddef.h>

struct channel_node {
	size_t size;
	void *data;
	void (*free)(void *data);
	struct channel_node *next;
};

struct channel {
	size_t length;
	struct channel_node *front;
	struct channel_node *rear;
};

struct channel *channel_init(void);
int channel_is_empty(struct channel *c);
void *channel_peek(struct channel *c);
void channel_send(struct channel *c, size_t size, void *data, void (*free)(void *data));
void *channel_recv(struct channel *c);
void channel_free(struct channel *c);

#endif // _CHANNEL_H
