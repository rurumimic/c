#include "wakers.h"

#include <stdio.h>
#include <stdlib.h>

struct wakers *wakers_init(size_t capacity)
{
	if (capacity == 0) {
		perror("wakers_init: capacity must be greater than 0");
		return NULL;
	}

	struct wakers *wakers = (struct wakers *)malloc(sizeof(struct wakers));

	if (!wakers) {
		perror("wakers_init: malloc failed to allocate waekers");
		exit(EXIT_FAILURE);
	}

	wakers->capacity = capacity;
	wakers->length = 0;
	wakers->nodes = (struct wakers_node *)malloc(
		sizeof(struct wakers_node) * capacity);

	if (!wakers->nodes) {
		perror("wakers_init: malloc failed to allocate wakers->nodes");
		exit(EXIT_FAILURE);
	}

	return wakers;
}

int wakers_hash_function(int key, size_t capacity)
{
	if (capacity == 0) {
		perror("wakers_hash_function: capacity must be greater than 0");
		return -1;
	}

	return key % capacity;
}

int wakers_insert(struct wakers *w, int key, void *data,
		  void (*free)(void *data))
{
	if (!w) {
		perror("wakers_insert: wakers is NULL");
		return -1;
	}

	if (w->length == w->capacity) {
		perror("wakers_insert: wakers is full");
		return -1;
	}

	int index = wakers_hash_function(key, w->capacity);

	if (index < 0 || index >= w->capacity) {
		perror("wakers_insert: index is out of bounds");
		return -1;
	}

	int i = index;

	do {
		struct wakers_node *node = &w->nodes[i];

		if (node->state == WAKERS_NODE_EMPTY ||
		    node->state == WAKERS_NODE_DELETED) {
			node->state = WAKERS_NODE_USED;
			node->key = key;
			node->data = data;
			node->free = free;
			w->length++;
			return 1;
		}

		if (node->key == key) {
			perror("wakers_insert: key is already used");
			return 0;
		}

		i = wakers_hash_function(i + 1, w->capacity);

	} while (i != index);

	perror("wakers_insert: wakers is full");
	return 0;
}

struct wakers_node *wakers_find(struct wakers *w, int key)
{
	if (!w) {
		perror("wakers_find: wakers is NULL");
		return NULL;
	}

	int index = wakers_hash_function(key, w->capacity);

	if (index < 0 || index >= w->capacity) {
		perror("wakers_find: index is out of bounds");
		return NULL;
	}

	int i = index;

	do {
		struct wakers_node *node = &w->nodes[i];

		if (node->state == WAKERS_NODE_EMPTY) {
			return NULL;
		}

		if (node->state == WAKERS_NODE_USED && node->key == key) {
			return node;
		}

		i = wakers_hash_function(i + 1, w->capacity);

	} while (i != index);

	return NULL;
}

void *wakers_remove(struct wakers *w, int key)
{
	if (!w) {
		perror("wakers_remove: wakers is NULL");
		return NULL;
	}

	struct wakers_node *node = wakers_find(w, key);
	if (!node) {
		return NULL;
	}

	void *data = node->data;

	node->state = WAKERS_NODE_DELETED;
	node->key = -1;
	node->data = NULL;
	node->free = NULL;

	w->length--;

	return data;
}

void wakers_free(struct wakers *w)
{
	if (!w) {
		perror("wakers_free: wakers is NULL");
		return;
	}

	struct wakers_node *node = w->nodes;

	for (size_t i = 0; i < w->length; i++) {
		if (node->data) {
			node->free(node->data);
		}
		node++;
	}

	free(w->nodes);
	free(w);
}