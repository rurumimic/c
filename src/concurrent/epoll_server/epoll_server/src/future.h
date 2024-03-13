#ifndef _FUTURE_H
#define _FUTURE_H

#include "channel.h"

enum poll_state {
	POLL_PENDING,
	POLL_READY,
};

struct future {
	enum poll_state (*poll)(struct future *f, struct channel *c);
	void *data;
  void (*free)(struct future *f);
};

#endif // _FUTURE_H
