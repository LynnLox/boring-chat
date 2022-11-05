/* server -- the friggin' server Himself */

#ifndef SERVER
#define SERVER

#include <poll.h>

void add_pfd(struct pollfd *pfds[], int *fd_cnt, int *fd_size, int newfd);
void rem_pfd(struct pollfd *pfds[], int *fd_cnt, int *fd_size, int i);
void handle_msg(char *msg, const int senderfd);
void handle_client_msg(char *msg, const int senderfd);
void handle_usr_msg(char *msg);

#endif
