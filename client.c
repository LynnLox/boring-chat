#include "net.h"
#include "user.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT "9034"

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "There must be 2 arguments (only)\n");
		exit(1);
	}
	struct addrinfo *list = init_addrinfos(-1, AF_UNSPEC, SOCK_STREAM, argv[1], PORT);
	int sockfd;
	struct addrinfo *addr = bind_socket(list, &sockfd, 0);
	if (sockfd == -1) {
		fprintf(stderr, "No valid addrinfo struct\n");
		exit(1);
	} else if (!addr) {
		fprintf(stderr, "Socket could not connect to server\n");
		exit(1);
	}

	while (1) {
		/* placeholder */
	}

	return 0;
}
