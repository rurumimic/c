#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);

	// Allow the socket to be reused immediately after the server is closed
	int opt = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET; // IPv4
	server_address.sin_port = htons(9000);
	server_address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0: Address to accept any incoming messages.

	bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

  printf("$ telnet localhost 9000\n");

	listen(server_socket, 4);

	int client_socket = accept(server_socket, NULL, NULL);

  printf("Connected.\n");

	char server_message[256] = "Hello, World!\n";
	send(client_socket, server_message, sizeof(server_message), 0);

  printf("Bye.\n");
	close(server_socket);

	return 0;
}
