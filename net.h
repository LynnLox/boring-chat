/* net -- deals with low-level networking details */

#ifndef NET
#define NET

#include <sys/types.h>
#include <sys/socket.h>

void *get_in_addr(struct sockaddr *sa);
struct addrinfo *init_addrinfos(int flags, int family, int socktype,
		const char *node, const char *port);
int bind_socket(struct addrinfo *list);

#endif
