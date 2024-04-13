#include "wakers.h"
#include "task.h"

#include <assert.h>
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
		perror("wakers_init: malloc failed to allocate wakers");
		exit(EXIT_FAILURE);
	}

	wakers->capacity = capacity;
	wakers->length = 0;
	wakers->nodes = (struct wakers_node *)malloc(
		capacity * sizeof(struct wakers_node));

	if (!wakers->nodes) {
		perror("wakers_init: malloc failed to allocate wakers->nodes");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < capacity; i++) {
		wakers->nodes[i].state = WAKERS_NODE_EMPTY;
		wakers->nodes[i].key = -1;
		wakers->nodes[i].waker = (struct waker){ .ptr = NULL,
							 .wake = NULL,
							 .free = NULL };
	}

	return wakers;
}

void wakers_free(struct wakers *wakers)
{
  assert(wakers != NULL);

	for (size_t i = 0; i < wakers->capacity; i++) {
		if (wakers->nodes[i].state == WAKERS_NODE_USED) {
			wakers->nodes[i].waker.free(wakers->nodes[i].waker.ptr);
		}
	}

	free(wakers->nodes);
	free(wakers);
}

int wakers_hash_function(int key, size_t capacity)
{
	if (capacity == 0) {
		perror("wakers_hash_function: capacity must be greater than 0");
		return -1;
	}

	return key % capacity;
}

int wakers_insert(struct wakers *wakers, int key, struct waker waker)
{
  assert(wakers != NULL);

	if (wakers->length == wakers->capacity) {
		perror("wakers_insert: wakers is full");
		return -1;
	}

	int index = wakers_hash_function(key, wakers->capacity);

	if (index < 0 || index >= wakers->capacity) {
		perror("wakers_insert: index is out of bounds");
		return -1;
	}

	int i = index;

	do {
		struct wakers_node *node = &wakers->nodes[i];

		if (node->state == WAKERS_NODE_EMPTY ||
		    node->state == WAKERS_NODE_DELETED) {
			node->state = WAKERS_NODE_USED;
			node->key = key;
			node->waker = waker;

			wakers->length++;
			return 1;
		}

		if (node->key == key) {
			perror("wakers_insert: key is already used");
			return 0;
		}

		i = wakers_hash_function(i + 1, wakers->capacity);

	} while (i != index);

	perror("wakers_insert: wakers is full");
	return 0;
}

struct wakers_node *wakers_find(struct wakers *wakers, int key)
{
  assert(wakers != NULL);

	int index = wakers_hash_function(key, wakers->capacity);

	if (index < 0 || index >= wakers->capacity) {
		perror("wakers_find: index is out of bounds");
		return NULL;
	}

	int i = index;

	do {
		struct wakers_node *node = &wakers->nodes[i];

		if (node->state == WAKERS_NODE_EMPTY) {
			return NULL;
		}

		if (node->state == WAKERS_NODE_USED && node->key == key) {
			return node;
		}

		i = wakers_hash_function(i + 1, wakers->capacity);

	} while (i != index);

	return NULL;
}

struct waker wakers_remove(struct wakers *w, int key)
{
	struct waker empty_waker = { .ptr = NULL, .wake = NULL, .free = NULL };

  assert(w != NULL);

	struct wakers_node *node = wakers_find(w, key);
	if (!node) {
		return empty_waker;
	}

	struct waker waker = node->waker;

	node->state = WAKERS_NODE_DELETED;
	node->key = -1;
	node->waker = empty_waker;

	w->length--;

	return waker;
}
