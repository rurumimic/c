#include "../future.h"
#include "../futures/accept.h"
#include "../scheduler/io_selector.h"
#include "async_listener.h"
#include "async_reader.h"

#include <arpa/inet.h>
#include <assert.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

struct async_listener *async_listener_init(int port,
					   struct io_selector *selector)
{
	assert(selector != NULL);

	struct async_listener *listener =
		(struct async_listener *)malloc(sizeof(struct async_listener));
	if (!listener) {
		perror("async_listener_init: malloc failed to allocate async_listener");
		return NULL;
	}

	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd < 0) {
		perror("async_listener_init: socket failed to create");
		free(listener);
		return NULL;
	}

	int opt = 1;
	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("async_listener_init: setsockopt failed to set options");
		shutdown(sfd, SHUT_RDWR);
		close(sfd);
		free(listener);
		return NULL;
	}

	int flags = fcntl(sfd, F_GETFL, 0);
	if (flags < 0) {
		perror("async_listener_init: fcntl failed to get flags");
		shutdown(sfd, SHUT_RDWR);
		close(sfd);
		free(listener);
		return NULL;
	}
	if (fcntl(sfd, F_SETFL, flags | O_NONBLOCK) < 0) {
		perror("async_listener_init: fcntl failed to set flags");
		shutdown(sfd, SHUT_RDWR);
		close(sfd);
		free(listener);
		return NULL;
	}

	struct sockaddr_in address;
	address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
	address.sin_family = AF_INET; // IPv4
	address.sin_port = htons(port);

	if (bind(sfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("async_listener_init: bind failed");
		shutdown(sfd, SHUT_RDWR);
		close(sfd);
		free(listener);
		return NULL;
	}

	listener->sfd = sfd;
	listener->selector = selector;

	return listener;
}

void async_listener_free(struct async_listener *listener)
{
	assert(listener != NULL);

	// move sfd to io_selector
	io_selector_unregister(listener->selector, listener->sfd);
	free(listener);
}

struct future *async_listener_accept(struct async_listener *listener)
{
	assert(listener != NULL);

	return accept_init(listener->selector, listener->sfd);
}
