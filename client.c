#include "client.h"
#include "net.h"
#include "user.h"
#include "message.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT "9034"
#define CON_LEN 250 /* content length, where content is what the user types */
#define VAL_LEN 15
#define SUB_LEN 7
#define PAD_LEN 2 /* for '!'/'@' and ':' */
#define USR_MSG_LEN (CON_LEN + PAD_LEN + VAL_LEN) /*length of user messages */
#define CLI_MSG_LEN (SUB_LEN + PAD_LEN + VAL_LEN) /* length of client messages */

/* calling this disgrace a "form" is a stretch, but I am bad with names */
void login_form(char *name)
{
	printf("Enter preferred alias: ");
	fgets(name, VAL_LEN, stdin);
}

char *login(const int sockfd)
{
	char name[VAL_LEN], res[VAL_LEN], msg[CLI_MSG_LEN];
	int flag = 1;
	while (flag) {
		login_form(name);
		trim_str(name);
		pack_client_msg(msg, name, CNAME);
		if (send(sockfd, msg, strlen(msg), 0) == -1) {
			perror("send");
			exit(1);
		}
		bzero(msg, CLI_MSG_LEN);
		if (recv(sockfd, msg, CLI_MSG_LEN, 0) == -1) {
			perror("recv");
			exit(1);
		}
		unpack_client_msg(msg, res);
		if (!strcmp(res, "exists\n"))
			fprintf(stderr, "Sorry, the alias is taken. Please choose another.\n\n");
		else if (!strcmp(res, "long\n"))
			fprintf(stderr, "The alias is too long. Please choose a shorter one.\n\n");
		else
			flag = 0;
	}
	printf("The alias is assigned.\n\n");
	return name;
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
	
	char *name = login(sockfd);

	char buf[CON_LEN], msg[USR_MSG_LEN];
	while (1) {
		printf("> ");
		fgets(buf, CON_LEN, stdin);
		pack_usr_msg(msg, buf, name);
		if (send(sockfd, msg, strlen(msg), 0) == -1) {
			perror("send");
			exit(1);
		}
		bzero(buf, CON_LEN);
		bzero(msg, USR_MSG_LEN);
	}

	return 0;
}
