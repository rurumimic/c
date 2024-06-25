#ifndef _CHANNEL_H
#define _CHANNEL_H

#include <pthread.h>
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
	pthread_mutex_t cond_mutex;
	pthread_cond_t cond;
};

struct channel *channel_init(void);
void channel_free(struct channel *channel);

int channel_is_empty(struct channel *channel);
struct task *channel_peek(struct channel *channel);
int channel_send(struct channel *channel, struct task *task);
struct task *channel_recv(struct channel *channel);

#endif // _CHANNEL_H
