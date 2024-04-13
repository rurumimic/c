#include "channel.h"
#include "task.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct channel *channel_init(void)
{
	struct channel *channel =
		(struct channel *)malloc(sizeof(struct channel));

	if (!channel) {
		perror("channel_init: malloc failed to allocate channel");
		exit(EXIT_FAILURE);
	}

	channel->front = NULL;
	channel->rear = NULL;
	channel->length = 0;
	channel->cond_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	channel->cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

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

	pthread_mutex_destroy(&channel->cond_mutex);
	pthread_cond_destroy(&channel->cond);

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

	pthread_mutex_lock(&channel->cond_mutex);
	while (channel_is_empty(channel)) {
		if (pthread_cond_wait(&channel->cond, &channel->cond_mutex) !=
		    0) {
			perror("channel_recv: pthread_cond_wait failed");
			pthread_mutex_unlock(&channel->cond_mutex);
			return NULL;
		}
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

	pthread_mutex_unlock(&channel->cond_mutex);

	return task;
}
