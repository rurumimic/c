#include "executor.h"
#include "channel.h"
#include "future.h"
#include "spawner.h"
#include "task.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct executor *executor_init(void) {
    struct executor *e = (struct executor *)malloc(sizeof(struct executor));

    if (!e) {
        perror("executor_init: malloc failed to allocate executor");
        exit(EXIT_FAILURE);
    }

    e->channel = channel_init();

    return e;
}

struct spawner *executor_get_spawner(struct executor *e) {
    if (!e) {
        perror("executor_get_spawner: executor is NULL");
        return NULL;
    }

    return spawner_init(e->channel);
}

void executor_run(struct executor *e) {
    if (!e) {
        perror("executor_run: executor is NULL");
        return;
    }

    printf("run loop:\n");
    while (!channel_is_empty(e->channel)) {
        struct task *t = (struct task *)channel_recv(e->channel);

        if (!t) {
            perror("executor_run: task future is NULL");
            exit(1);
        }

        if (!t->future) {
            perror("executor_run: task future is NULL");
            exit(1);
        }

        pthread_mutex_lock(&t->future_mutex);

        enum poll_state state = t->future->poll(t->future, e->channel);

        pthread_mutex_unlock(&t->future_mutex);

        if (state == POLL_PENDING) {
            channel_send(e->channel, sizeof(struct task), t, task_free);
            continue;
        }

        if (state == POLL_READY && t->future->state == FUTURE_STOPPED) {
            task_free(t);
        }
  }
}

void executor_free(struct executor *e) {
    if (!e) {
        perror("executor_free: executor is NULL");
        return;
    }

    if (e->channel) {
        channel_free(e->channel);
    }

    free(e);
}
