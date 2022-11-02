#include "client.h"
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
#define MSG_LEN 256
#define NAME_LEN 15

/* calling this disgrace a "form" is a stretch, but I am bad with names */
void login_form(char *name)
{
	printf("Enter preferred alias: ");
	fgets(name, NAME_LEN, stdin);
}

void login(const int sockfd)
{
	char name[MSG_LEN], res[20];
	int flag = 1;
	while (flag) {
		login_form(name);
		if (send(sockfd, name, strlen(name), 0) == -1) {
			perror("send");
			exit(1);
		}
		bzero(res, 20);
		if (recv(sockfd, res, 20, 0) == -1) {
			perror("recv");
			exit(1);
		}
		if (!strcmp(res, "exists\n"))
			fprintf(stderr, "Sorry, the alias is taken. Please choose another.\n\n");
		else if (!strcmp(res, "long\n"))
			fprintf(stderr, "The alias is too long. Please choose a shorter one.\n\n");
		else
			flag = 0;
	}
	printf("The alias is assigned.\n\n");
}

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
	
	login(sockfd);

	char buf[MSG_LEN];
	while (1) {
		printf("> ");
		fgets(buf, MSG_LEN, stdin);
		if (send(sockfd, buf, strlen(buf), 0) == -1) {
			perror("send");
			exit(1);
		}
		bzero(buf, MSG_LEN);
	}

	return 0;
}
