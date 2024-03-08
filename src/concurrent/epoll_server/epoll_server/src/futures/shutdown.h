#ifndef _SHUTDOWN_H
#define _SHUTDOWN_H

#include "../future.h"
#include "../channel.h"
#include "../io_selector.h"

struct shutdown_data {
  // ref
  struct io_selector *selector;
};

struct future *shutdown_init(struct io_selector *selector);
void shutdown_free(struct future *f);

enum poll_state shutdown_poll(struct future *f, struct channel *c);

#endif // !_SHUTDOWN_H
