#ifndef _GOODBYE_H
#define _GOODBYE_H

#include "future.h"
#include "poll.h"

#define GOODBYE_SIZE 10

struct goodbye {
	char *s;
};

struct future *goodbye_init(void);
void goodbye_free(struct future *future);
struct poll goodbye_poll(struct future *future, struct context cx);

#endif // _GOODBYE_H
