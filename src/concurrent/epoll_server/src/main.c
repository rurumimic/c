#include <stdlib.h>

#include "hello.h"
#include "future.h"

int main(int argc, char *argv[])
{
	hello *h = malloc(sizeof(hello));
	h->state = HELLO;

	poll(h);
	poll(h);
	poll(h);

	return 0;
}
