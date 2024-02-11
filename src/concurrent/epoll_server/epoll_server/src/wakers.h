#ifndef _WAKERS_H
#define _WAKERS_H

#include <stddef.h>

enum wakers_node_state {
	WAKERS_NODE_EMPTY,
	WAKERS_NODE_USED,
	WAKERS_NODE_DELETED
};

struct wakers_node {
	enum wakers_node_state state;
	int key;
	void *data;
	void (*free)(void *data);
};

struct wakers {
	struct wakers_node *nodes;
	size_t length;
	size_t capacity;
};

struct wakers *wakers_init(size_t capacity);
int wakers_insert(struct wakers *w, int key, void *data,
		  void (*free)(void *data));
struct wakers_node *wakers_find(struct wakers *w, int key);
void *wakers_remove(struct wakers *w, int key);
void wakers_free(struct wakers *w);

#endif // _WAKERS_H