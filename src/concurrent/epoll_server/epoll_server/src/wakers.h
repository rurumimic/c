#ifndef _WAKERS_H
#define _WAKERS_H

#include <stddef.h>

#include "task.h"

enum wakers_node_state {
	WAKERS_NODE_EMPTY,
	WAKERS_NODE_USED,
	WAKERS_NODE_DELETED
};

struct waker {
  struct task *task;
};

struct wakers_node {
	enum wakers_node_state state;
	int key;
  struct waker *waker;
};

struct wakers {
	struct wakers_node *nodes;
	size_t length;
	size_t capacity;
};

struct wakers *wakers_init(size_t capacity);
struct waker *waker_init(struct task *task);
int wakers_insert(struct wakers *w, int key, struct waker *waker);
struct wakers_node *wakers_find(struct wakers *w, int key);
struct waker *wakers_remove(struct wakers *w, int key);
void waker_free(struct waker *w);
void wakers_free(struct wakers *w);

#endif // _WAKERS_H
