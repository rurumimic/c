#ifndef _FUTURE_H
#define _FUTURE_H

#include "hello.h"

enum poll_state {
	POLL_PENDING,
	POLL_READY,
};

enum poll_state poll(struct hello *h);

#endif // _FUTURE_H
