#include "future.h"

struct context from_waker(struct waker waker)
{
	return (struct context){ .waker = waker };
}
