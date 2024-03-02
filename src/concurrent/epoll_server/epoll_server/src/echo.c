#include "echo.h"
#include "async_listener.h"
#include "async_reader.h"
#include "future.h"

#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct future *echo_init(struct async_listener *listener, struct async_reader *reader, int fd) {
  printf("echo");
  struct future *f = (struct future *)malloc(sizeof(struct future));

  if (!f) {
    perror("echo_init: malloc failed to allocate echo");
    exit(EXIT_FAILURE);
  }

  struct echo_data *data = (struct echo_data *)malloc(sizeof(struct echo_data));
  data->listener = listener;
  data->reader = reader;
  data->fd = fd;
  data->reader_future = NULL;

  f->state = FUTURE_INIT;
  f->poll = NULL;
  f->data = data;

  return f;
}

enum poll_state echo_poll(struct future *f, struct channel *c) {
  struct echo_data *data = (struct echo_data *)f->data;
  struct async_listener *listener = data->listener;
  struct async_reader *reader = data->reader;
  int fd = data->fd;
  struct future *reader_future = data->reader_future;

  printf("echo_poll\n");

  if (reader_future == NULL) {
    data->reader_future = async_reader_readline(reader);
    struct task *reader_task = task_init(data->reader_future, c);
    channel_send(c, sizeof(struct task), reader_task, task_free);

    return POLL_PENDING;
  }

  if (reader_future->state == FUTURE_INIT) {
    return POLL_PENDING;
  }

  data->reader_future->state = FUTURE_STOPPED;
  data->reader_future = NULL;

  struct reader_data *result = (struct reader_data *)reader_future->data;

  char *lines = result->lines;

  if (lines == NULL) {
    async_reader_free(reader);
    printf("close: addr\n");
    return POLL_READY;
  }

  printf("read: %s, addr\n", lines);

  write(fd, lines, strlen(lines));
  fsync(fd);

  return POLL_PENDING;
}

