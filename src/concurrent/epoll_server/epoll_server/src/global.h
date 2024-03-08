#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <signal.h>
#include <pthread.h>

extern volatile sig_atomic_t running;

extern pthread_mutex_t cond_mutex;
extern pthread_cond_t cond;

#endif // _GLOBAL_H
