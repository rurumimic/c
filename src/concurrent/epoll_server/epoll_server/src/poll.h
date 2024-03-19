#ifndef _POLL_H
#define _POLL_H

enum poll_state {
	POLL_PENDING,
	POLL_READY,
};

struct poll {
	enum poll_state state;
	void *output; // when state == POLL_READY, output is valid
	void (*free)(void *output);
};

#endif // _POLL_H
