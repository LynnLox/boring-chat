/* client -- the program that the user runs to connect to the server and chat */

#ifndef CLIENT
#define CLIENT

void login_form(char *name);
char *login(const int sockfd);

#endif
