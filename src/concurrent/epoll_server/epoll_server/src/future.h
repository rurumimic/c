#ifndef _FUTURE_H
#define _FUTURE_H

#include "channel.h"
#include "async_reader.h"

enum future_state {
  FUTURE_INIT,
  FUTURE_RUNNING,
  FUTURE_STOPPED,
};

enum poll_state {
  POLL_PENDING,
    POLL_READY,
};

struct future {
  enum future_state state;
  enum poll_state (*poll)(struct future *f, struct channel *c);
  void *data;
};

#endif // _FUTURE_H
