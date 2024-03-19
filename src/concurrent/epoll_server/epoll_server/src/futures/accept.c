#include "accept.h"
#include "../async_listener.h"
#include "../future.h"
#include "../io_selector.h"
#include "../channel.h"
#include "../async_reader.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>

struct future *accept_init(struct io_selector *selector, int sfd)
{
  if (!selector) {
    perror("accept_init: selector is NULL");
    exit(EXIT_FAILURE);
  }

	struct future *f = (struct future *)malloc(sizeof(struct future));

	if (!f) {
		perror("accept_init: malloc failed to allocate future");
		exit(EXIT_FAILURE);
	}

	struct accept_data *data = (struct accept_data *)malloc(sizeof(struct accept_data));

	if (!data) {
		perror("accpet_init: malloc failed to allocate accept_data");
		exit(EXIT_FAILURE);
	}

	data->selector = selector;
	data->sfd = sfd;
	data->cfd = 0;
	memset(data->cip, 0, INET_ADDRSTRLEN);

	f->data = data;
	f->poll = accept_poll;
	f->free = accept_free;

	return f;
}

void accept_free(struct future *f)
{
	if (!f) {
		perror("accept_free: future is NULL");
		return;
	}

	struct accept_data *data = (struct accept_data *)f->data;

	if (data) {
		free(data);
	} else {
		perror("accept_free: data is NULL");
	}

	free(f);
}

struct poll accept_poll(struct future *f, struct context cx)
{
	if (!f) {
		perror("accept_poll: future is NULL");
		exit(EXIT_FAILURE);
	}

	struct accept_data *data = (struct accept_data *)f->data;
	struct io_selector *selector = data->selector;
	int sfd = data->sfd;

	struct sockaddr_in client_address;
	size_t client_addr_size = sizeof(client_address);

	errno = 0;
	int cfd = accept(sfd, (struct sockaddr *)&client_address, (socklen_t *)&client_addr_size);
	if (cfd < 0) {
		if (errno == EWOULDBLOCK) {
			io_selector_register(selector, EPOLLIN, sfd, cx.waker);
			return (struct poll){.state = POLL_PENDING, .output = NULL, .free = NULL};
		} else {
			perror("accept_poll: accept failed");
			exit(EXIT_FAILURE);
		}
	}

	data->cfd = cfd;
	if (inet_ntop(AF_INET, &client_address.sin_addr, data->cip, INET_ADDRSTRLEN) == NULL) {
		perror("accept_poll: inet_ntop failed");
		exit(EXIT_FAILURE);
	}

	return (struct poll){.state = POLL_READY, .output = data, .free = NULL};
}
