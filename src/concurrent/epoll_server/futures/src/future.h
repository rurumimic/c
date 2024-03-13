#ifndef _FUTURE_H
#define _FUTURE_H

#include "poll.h"

struct waker {
  void (*wake)(void *data);
  void (*wake_by_ref)(void *data);
  void (*drop)(void *data);
};

struct context {
  struct waker *waker;
  void *data;
};

struct future {
  struct poll (*poll)(struct future *future, struct context *cx);
  void (*drop)(struct future *future);
};

struct poll poll(struct future *future, struct context *cx);

struct context from_waker(struct waker *waker);
struct waker *waker(struct context *cx);

#endif // _FUTURE_H
