#include "net.h"
#include "user.h"
#include "message.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT "9034"
#define BACKLOG 10

#define CON_LEN 250
#define PAD_LEN 2
#define VAL_LEN 15
#define USR_MSG_LEN (CON_LEN + PAD_LEN + VAL_LEN)

void add_pfd(struct pollfd *pfds[], int *fd_cnt, int *fd_size, int newfd)
{
	if (*fd_cnt == *fd_size) {
		*fd_size *= 2;
		*pfds = realloc(*pfds, sizeof(**pfds) * *fd_size);
	}
	(*pfds)[*fd_cnt].fd = newfd;
	(*pfds)[*fd_cnt].events = POLLIN;
	(*fd_cnt)++;
}

void rem_pfd(struct pollfd *pfds[], int *fd_cnt, int *fd_size, int i)
{
	pfds[i] = pfds[*fd_cnt - 1];
	(*fd_cnt)--;
}

int main()
{
	struct addrinfo *list = init_addrinfos(AI_PASSIVE, AF_UNSPEC, SOCK_STREAM, NULL, PORT);
	int sockfd;
	bind_socket(list, &sockfd, 1);
	if (sockfd == -1) {
		fprintf(stderr, "No valid socket found\n");
		exit(1);
	} else if (sockfd == -2) {
		fprintf(stderr, "Socket flags could not be set.\n");
		exit(1);
	} else if (listen(sockfd, BACKLOG) == -1) {
		fprintf(stderr, "Error getting listening socket.\n");
		exit(1);
	}

	int fd_cnt = 0, fd_size = 5;
	struct pollfd *pfds = malloc(sizeof(*pfds) * fd_size);
	add_pfd(&pfds, &fd_cnt, &fd_size, sockfd);

	init_users_list(fd_size);

	while (1) {
		int poll_cnt = poll(pfds, fd_cnt, -1);
		if (poll_cnt == -1) {
			perror("poll");
			exit(1);
		}
		for (int i = 0; i < fd_cnt; ++i) {
			if (pfds[i].revents & POLLIN) {
				if (pfds[i].fd == sockfd) {
					struct sockaddr_storage ss;
					int ss_len = sizeof(ss);
					int connfd = accept(sockfd, (struct sockaddr*)&ss, &ss_len);
					if (connfd == -1) {
						perror("accept");
					} else {
						add_pfd(&pfds, &fd_cnt, &fd_size, connfd);
						char remoteIP[INET6_ADDRSTRLEN];
						printf("pollserver: new connection from %s on socket %d\n",
								inet_ntop(ss.ss_family,	get_in_addr((struct sockaddr*)&ss), remoteIP,
									INET6_ADDRSTRLEN), connfd);
					}
				} else {
					char buf[CON_LEN], msg[USR_MSG_LEN];
					int nbytes = recv(pfds[i].fd, msg, sizeof(msg), 0);
					int senderfd = pfds[i].fd;
					if(!nbytes) {
						printf("pollserver: connection closed by socket %d\n", senderfd);
						close(pfds[i].fd);
					} else {
						printf("%s\n", msg);
						unpack_client_msg(msg, buf);
						printf("%s\n", buf);
						int status = add_user(buf);
						char res[VAL_LEN];
						if (!status)
							strcpy(res, "success");
						else if (status == -1)
							strcpy(res, "capfull");
						else if (status == -2)
							strcpy(res, "long");
						else if (status == -3)
							strcpy(res, "exists");
						trim_str(res);
						pack_client_msg(msg, res, CNAME);
						if (send(senderfd, msg, strlen(msg), 0) == -1)
							perror("send");
					}
				}
			}
		}
	}
	return 0;
}
