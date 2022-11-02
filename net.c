#include "net.h"

#include <string.h>
#include <netdb.h>

void *get_in_addr(struct sockaddr *sa)
{
	return sa->sa_family == AF_INET ? &(((struct sockaddr_in*)sa)->sin_addr)
		: &(((struct sockaddr_in6*)sa)->sin6_addr);
}

struct addrinfo *init_addrinfos(int flags, int family, int socktype,
		const char *node, const char *port)
{
	struct addrinfo hints, *list;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = family;
	hints.ai_socktype = socktype;
	if (flags != -1)
		hints.ai_flags = flags;
	return !getaddrinfo(node, port, &hints, &list) ? list : NULL;
}

struct addrinfo *bind_socket(struct addrinfo *list, int *sockfd, const int is_serv)
{
	for (struct addrinfo *p = list; p; p = p->ai_next) {
		if ((*sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
			continue;
		if (is_serv) {
			int yes = 1;
			if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
				*sockfd = -1;
				return NULL;
			}
			if (bind(*sockfd, p->ai_addr, p->ai_addrlen) == -1)
				continue;
		} else {
			if (connect(*sockfd, p->ai_addr, p->ai_addrlen) == -1)
				continue;
		}
		return p;
	}

	/* nothing in the list was valid */

	/* This is done for server only, since the server depends on sock_fd
	 * value for error checking, while the client looks at the struct
	 * addrinfo returned. It is possible for the client to return NULL
	 * despite having got a socket(), if connect() then failed.
	 */
	if (is_serv)
		*sockfd = -1;
	freeaddrinfo(list);

	return NULL;
}
