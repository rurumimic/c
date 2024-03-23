#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int repeat = 3;
	int result;
	int flags;
	int client_socket;
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);

	// Allow the socket to be reused immediately after the server is closed
	int opt = 1;
	result = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt,
			    sizeof(opt));
	if (result < 0) {
		perror("setsockopt");
		return 1;
	}

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET; // IPv4
	server_address.sin_port = htons(9000);
	server_address.sin_addr.s_addr =
		INADDR_ANY; // 0.0.0.0: Address to accept any incoming messages.

	result = bind(server_socket, (struct sockaddr *)&server_address,
		      sizeof(server_address));
	if (result < 0) {
		perror("bind");
		return 1;
	}

	printf("$ telnet localhost 9000\n");

	result = listen(server_socket, 4);
	if (result < 0) {
		perror("listen");
		return 1;
	}

	flags = fcntl(server_socket, F_GETFL, 0);
	if (flags < 0) {
		perror("fcntl");
		return 1;
	}

	flags |= O_NONBLOCK;
	result = fcntl(server_socket, F_SETFL, flags);
	if (result < 0) {
		perror("fcntl");
		return 1;
	}

	for (int i = 0; i < repeat;) {
		errno = 0;
		client_socket = accept(server_socket, NULL, NULL);

		if (client_socket < 0) {
			if (errno == EWOULDBLOCK) {
				continue;
			} else {
				perror("accept");
				return 1;
			}
		} else {
			printf("Connected: (%d/%d)\n", ++i, repeat);

			errno = 0;
			char buffer[256] = {
				0,
			};

			if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1) {
				perror("main: fcntl failed to set O_NONBLOCK");
				return 1;
			}

			while (1) {
				errno = 0;
				ssize_t n =
					read(client_socket, buffer, 256 - 1);
				if (n == -1) {
					if (errno == EINTR) {
					} else if (errno == EAGAIN) {
					} else if (errno == EWOULDBLOCK) {
					} else {
						perror("main: read failed to read from stdin");
						return 1;
					}
				} else {
					break;
				}
			}

			char server_message[256] = {
				0,
			};
			sprintf(server_message, "Echo (%d/%d): %s\n", i, repeat,
				buffer);

			send(client_socket, server_message,
			     sizeof(server_message), 0);

			printf("Bye.\n");

			shutdown(client_socket, SHUT_RDWR);
			close(client_socket);
		}
	}

	close(server_socket);

	return 0;
}
