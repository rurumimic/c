#include "channel.h"
#include "task.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct channel *channel_init(void)
{
	struct channel *channel = (struct channel *)malloc(sizeof(struct channel));

	if (!channel) {
		perror("channel_init: malloc failed to allocate channel");
		exit(EXIT_FAILURE);
	}

	channel->front = NULL;
	channel->rear = NULL;
	channel->length = 0;

	return channel;
}

void channel_free(struct channel *channel)
{
  assert(channel != NULL);

	struct channel_node *node = channel->front;

	while (node) {
		struct channel_node *next = node->next;

		if (node->task) {
			task_free(node->task);
		}

		free(node);

		node = next;
	}

	free(channel);
}

int channel_is_empty(struct channel *channel)
{
  assert(channel != NULL);

	return channel->length == 0;
}

struct task *channel_peek(struct channel *channel)
{
  assert(channel != NULL);

	if (!channel->front) {
		return NULL;
	}

	return channel->front->task;
}

void channel_send(struct channel *channel, struct task *task)
{
  assert(channel != NULL);
  assert(task != NULL);

	struct channel_node *node =
		(struct channel_node *)malloc(sizeof(struct channel_node));

	if (!node) {
		perror("malloc failed in channel_send");
		exit(EXIT_FAILURE);
	}

	node->task = task;
	node->next = NULL;

	if (channel->front) {
		channel->rear->next = node;
		channel->rear = node;
	} else {
		channel->front = node;
		channel->rear = node;
	}

	channel->length++;
}

struct task *channel_recv(struct channel *channel)
{
  assert(channel != NULL);

	struct channel_node *node = channel->front;

	if (!node) {
		return NULL;
	}

	struct task *task = node->task;

	channel->front = node->next;
	channel->length--;

	if (!channel->front) {
		channel->rear = NULL;
	}

	free(node);

	return task;
}
