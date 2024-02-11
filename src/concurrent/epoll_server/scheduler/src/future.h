#ifndef _FUTURE_H
#define _FUTURE_H

#include "task.h"

enum poll_state {
	POLL_PENDING,
	POLL_READY,
};

enum poll_state poll(struct task *t);

#endif // _FUTURE_H
