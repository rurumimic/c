#include "future.h"

#include <stdio.h>

enum poll_state poll(hello *hello)
{
	switch (hello->state) {
	case HELLO:
		printf("Hello, ");
		hello->state = WORLD;
		return POLL_PENDING;
	case WORLD:
		printf("World!\n");
		hello->state = END;
		return POLL_PENDING;
	default:
		return POLL_READY;
	}
}
