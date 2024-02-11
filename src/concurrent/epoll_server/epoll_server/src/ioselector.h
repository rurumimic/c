#ifndef _IOSELECTOR_H
#define _IOSELECTOR_H

#include <pthread.h>

struct io_selector {
	int epfd;
	int eventfd;

	pthread_mutex_t waker_mutex;
	pthread_mutex_t queue_mutex;
};

struct io_selector *io_selector_init();
void io_selector_free(struct io_selector *s);

#endif // _IOSELECTOR_H
