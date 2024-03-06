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

struct future *accept_init(struct future *server,
			   struct async_listener *listener)
{
	if (!server) {
		perror("accept_init: server is NULL");
		exit(EXIT_FAILURE);
	}

	if (!listener) {
		perror("accept_init: listener is NULL");
		exit(EXIT_FAILURE);
	}

	struct future *f = (struct future *)malloc(sizeof(struct future));

	if (!f) {
		perror("async_listener_accept: malloc failed to allocate future");
		exit(EXIT_FAILURE);
	}

	struct accept_data *data =
		(struct accept_data *)malloc(sizeof(struct accept_data));

	if (!data) {
		perror("async_listener_accept: malloc failed to allocate accept_data");
		exit(EXIT_FAILURE);
	}

	data->server = server; // waker
	data->sfd = listener->sfd;
	data->selector = listener->selector;
	data->cfd = 0;
	memset(data->cip, 0, INET_ADDRSTRLEN);

	f->poll = accept_poll;
	f->data = data;
	f->free = accept_free;

	return f;
}

void accept_free(struct future *f)
{
	if (!f) {
		perror("async_listener_accept_free: future is NULL");
		return;
	}

	struct accept_data *data = (struct accept_data *)f->data;

	if (data) {
		free(data);
	} else {
		perror("async_listener_accept_free: data is NULL");
	}

	free(f);
}

enum poll_state accept_poll(struct future *f, struct channel *c)
{
	if (!f) {
		perror("async_listener_accept_poll: future is NULL");
		exit(EXIT_FAILURE);
	}

	if (!c) {
		perror("async_listener_accept_poll: channel is NULL");
		exit(EXIT_FAILURE);
	}

	struct accept_data *data = (struct accept_data *)f->data;
	struct io_selector *selector = data->selector;
	struct future *server = data->server;
	int sfd = data->sfd;

	struct sockaddr_in client_address;
	size_t client_addr_size = sizeof(client_address);

	errno = 0;
	int cfd = accept(sfd, (struct sockaddr *)&client_address,
			 (socklen_t *)&client_addr_size);
	if (cfd < 0) {
		if (errno == EWOULDBLOCK) {
			io_selector_register(selector, EPOLLIN, sfd,
					     task_init(server, c));
			return POLL_PENDING;
		} else {
			perror("accept_poll: accept failed");
			exit(EXIT_FAILURE);
		}
	}

	data->cfd = cfd;
	if (inet_ntop(AF_INET, &client_address.sin_addr, data->cip,
		      INET_ADDRSTRLEN) == NULL) {
		perror("accept_poll: inet_ntop failed");
		exit(EXIT_FAILURE);
	}

	return POLL_READY;
}
