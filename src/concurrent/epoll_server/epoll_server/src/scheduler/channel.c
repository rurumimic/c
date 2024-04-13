#include "channel.h"
#include "task.h"

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
	if (!channel) {
		perror("channel_free: channel is NULL");
		return;
	}

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
	if (!channel) {
		perror("channel_is_empty: channel is NULL");
		return 1;
	}

	return channel->length == 0;
}

struct task *channel_peek(struct channel *channel)
{
	if (!channel) {
		return NULL;
	}

	if (!channel->front) {
		return NULL;
	}

	return channel->front->task;
}

void channel_send(struct channel *channel, struct task *task)
{
	if (!channel) {
		perror("channel_send: channel is NULL");
		return;
	}

	if (!task) {
		perror("channel_send: task is NULL");
		return;
	}

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
	if (!channel) {
		perror("channel_recv: channel is NULL");
		return NULL;
	}

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
