#include "../future.h"
#include "../io/async_listener.h"
#include "../io/async_reader.h"
#include "../scheduler/io_selector.h"
#include "../scheduler/channel.h"
#include "accept.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

struct future *accept_init(struct io_selector *selector, int sfd)
{
	assert(selector != NULL);

	struct future *future = (struct future *)malloc(sizeof(struct future));

	if (!future) {
		perror("accept_init: malloc failed to allocate future");
		return NULL;
	}

	struct accept_data *data =
		(struct accept_data *)malloc(sizeof(struct accept_data));

	if (!data) {
		perror("accpet_init: malloc failed to allocate accept_data");
		free(future);
		return NULL;
	}

	data->selector = selector;
	data->sfd = sfd;
	data->cfd = 0;
	memset(data->cip, 0, INET_ADDRSTRLEN);

	future->data = data;
	future->poll = accept_poll;
	future->free = accept_free;

	return future;
}

void accept_free(struct future *future)
{
	assert(future != NULL);

	struct accept_data *data = (struct accept_data *)future->data;

	if (data) {
		free(data);
	} else {
		perror("accept_free: data is NULL");
	}

	free(future);
}

struct poll accept_poll(struct future *future, struct context context)
{
	assert(future != NULL);

	struct accept_data *data = (struct accept_data *)future->data;
	if (!data) {
		perror("accept_poll: data is NULL");
    return (struct poll){ .state = POLL_READY,
                          .output = NULL,
                          .free = NULL };
	}

	struct io_selector *selector = data->selector;
	int sfd = data->sfd;

	struct sockaddr_in client_address;
	size_t client_addr_size = sizeof(client_address);

	errno = 0;
	int cfd = accept(sfd, (struct sockaddr *)&client_address,
			 (socklen_t *)&client_addr_size);
	if (cfd < 0) {
		if (errno == EWOULDBLOCK) {
			io_selector_register(selector, EPOLLIN, sfd,
					     context.waker);
			return (struct poll){ .state = POLL_PENDING,
					      .output = NULL,
					      .free = NULL };
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

	return (struct poll){ .state = POLL_READY,
			      .output = data,
			      .free = NULL };
}
