#include "echo.h"
#include "async_listener.h"
#include "async_reader.h"
#include "future.h"

#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct future *echo_init(struct async_listener *listener, struct async_reader *reader, int cfd) {
  printf("echo");
  struct future *f = (struct future *)malloc(sizeof(struct future));

  if (!f) {
    perror("echo_init: malloc failed to allocate echo");
    exit(EXIT_FAILURE);
  }

  struct echo_data *data = (struct echo_data *)malloc(sizeof(struct echo_data));
  data->listener = listener;
  data->reader = reader;
  data->cfd = cfd;
  data->readline = NULL;

  f->state = FUTURE_INIT;
  f->poll = NULL;
  f->data = data;

  return f;
}

enum poll_state echo_poll(struct future *f, struct channel *c) {
  struct echo_data *data = (struct echo_data *)f->data;
  struct async_listener *listener = data->listener;
  struct async_reader *reader = data->reader;

  printf("echo_poll\n");

  if (f->state == FUTURE_INIT) {
    data->readline = async_reader_readline(f, reader);
    f->state = FUTURE_RUNNING;
  }

  enum poll_state readline_state = data->readline->poll(data->readline, c);
  
  if (readline_state == POLL_PENDING) {
    return POLL_PENDING;
  }

  struct readline_data *result = (struct readline_data *)data->readline->data;

  if (result->len <= 0) {
    printf("close: %d\n", data->cfd);
    echo_data_free(data);
    return POLL_READY;
  }

  printf("read (%d): %s\n", data->cfd, result->lines);
  write(data->cfd, result->lines, result->len);
  fsync(data->cfd);

  return POLL_PENDING;
}

void echo_data_free(struct echo_data *data) {
  free(data->readline->data);
  free(data->readline);
  async_reader_free(data->reader);
  close(data->cfd);
  free(data);
}

