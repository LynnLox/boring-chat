/* client -- the program that the user runs to connect to the server and chat */

#ifndef CLIENT
#define CLIENT

void login_form();
void login(const int sockfd);

void thread_send(void *sfd);
void thread_recv(void *sfd);

#endif
