#ifndef _CHANNEL_H
#define _CHANNEL_H

#include <stddef.h>

struct task;

struct channel_node {
	struct task *task;
	struct channel_node *next;
};

struct channel {
	size_t length;
	struct channel_node *front;
	struct channel_node *rear;
};

struct channel *channel_init(void);
int channel_is_empty(struct channel *c);
struct task *channel_peek(struct channel *c);
void channel_send(struct channel *c, struct task *t);
struct task *channel_recv(struct channel *c);
void channel_free(struct channel *c);

#endif // _CHANNEL_H
