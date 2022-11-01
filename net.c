#include "net.h"

#include <string.h>

void *get_in_addr(struct sockaddr *sa)
{
	return sa->ai_family == AF_INET ? &(((struct sockaddr_in*)sa)->sin_addr)
		: &(((struct sockaddr_in6*)sa)->sin6_addr);
}

struct addrinfo *init_addrinfos(int flag, int family, int socktype,
		const char *node, const char *port)
{
	struct addrinfo hints, *list;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = family;
	hints.ai_socktype = socktype;
	if (flag != -1)
		hints.ai_flag = flag;
	return !getaddrinfo(node, port, &hints, &list) ? list : NULL;
}

int bind_socket(struct addrinfo *list)
{
	struct addrinfo *p;
	int sockfd;
	for (p = list; p; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0);
			continue;
		int yes = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
			return -2;
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
			continue;
		break;
	}

	/* nothing in the list was valid */
	if (!p)
		sockfd = -1;
	
	freeaddrinfo(list);

	return sockfd;
}
