#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define LINE_MAX 2048

int main(int argc, char *argv[])
{
	int fd = STDIN_FILENO;

	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		perror("main: fcntl failed to set O_NONBLOCK");
		exit(EXIT_FAILURE);
	}

	char *buffer = (char *)calloc(LINE_MAX, sizeof(char));

	if (buffer == NULL) {
		perror("main: calloc failed to allocate buffer");
		exit(EXIT_FAILURE);
	}

	while (1) {
		errno = 0;
		ssize_t n = read(0, buffer, LINE_MAX - 1);
		if (n == -1) {
			if (errno == EINTR) {
			} else if (errno == EAGAIN) {
			} else if (errno == EWOULDBLOCK) {
			} else {
				perror("main: read failed to read from stdin");
				exit(EXIT_FAILURE);
			}
		} else {
			break;
		}
	}

	buffer[LINE_MAX] = '\0';
	printf("read: %s\n", buffer);

	free(buffer);

	return 0;
}
