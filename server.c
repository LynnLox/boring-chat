#include "net.h"
#include "user.h"

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
#define MSG_MAX_LEN 15

void add_pfd(struct pollfd *pfds[], int *fd_cnt, int *fd_size, int newfd)
{
	if (*fd_cnt == *fd_size) {
		*fd_size *= 2;
		*pfds = realloc(*pfds, sizeof(**pfds), *fd_size);
	}
	(*pfds)[*fd_cnt].fd = newfd;
	(*pfds)[*fd_cnt].event = POLLIN;
	(*fd_cnt)++;
}

void rem_pfd(struct pollfd *pfds[], int *fd_cnt, int *fd_size, int i)
{
	pfds[i] = pfds[*fd_cnt - 1];
	(*fd_cnt)--;
}

int main()
{
	struct addrinfo *list = init_addrinfo(AI_PASSIVE, AI_UNSPEC, SOCK_STREAM, NULL, PORT);
	int sockfd = bind_socket(list);
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
						add_pfds(&pfds, &fd_cnt, &fd_size, connfd);
						char remoteIP[INET6_ADDRSTRLEN];
						printf("pollserver: new connection from %s on socket %d\n",
								inet_ntop(ss.ss_family, (struct sockaddr*)&ss,
									get_in_addr(struct sockaddr*)&ss, remoteIP,
									INET6_ADDRSTRLEN), connfd);
					}
				} else {
					char buf[MAX_MSG_LEN];
					int nbytes = recv(pfds[i].fd, buf, sizeof(buf));
					int senderfd = pfds[i].fd;
					if(!nbytes) {
						printf("pollserver: connection closed by socket %d\n", senderfd);
						close(pfds[i].fd);
					} else {
						int status = add_name(buf);
						char msg[20];
						if (!status)
							msg = "success";
						else if (status == -1)
							msg = "capfull";
						else if (status == -2)
							msg = "exists";
						if (send(senderfd, msg, 15, 0) == -1)
							perror("send");
					}
				}
			}
		}
	}
	return 0;
}
