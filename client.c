#include "client.h"
#include "net.h"
#include "user.h"
#include "message.h"
#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ncurses/ncurses.h>

#define PORT "9034"
#define CON_LEN 250 /* content length, where content is what the user types */
#define VAL_LEN 15
#define SUB_LEN 7
#define PAD_LEN 2 /* for '!'/'@' and ':' */
#define USR_MSG_LEN (CON_LEN + PAD_LEN + VAL_LEN) /*length of user messages */
#define CLI_MSG_LEN (SUB_LEN + PAD_LEN + VAL_LEN) /* length of client messages */

char name[VAL_LEN];
unsigned short flag = 1;
WINDOW *main_win, *msg_win, *msg_win_box, *ip_win, *ip_win_box, *login_win;

void login_form()
{
	char *login_prompt = "Enter preferred alias: ";
	wprintw(ip_win, login_prompt);
	wmove(ip_win, 0, strlen(login_prompt));
	wrefresh(ip_win);
	get_ip(name, VAL_LEN);
	wclear(ip_win);
	wrefresh(ip_win);
}

void login(const int sockfd)
{
	bzero(name, strlen(name));
	char res[VAL_LEN], msg[CLI_MSG_LEN];
	int flag = 1;
	char res_msg[CON_LEN];
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
			strcpy(res_msg, "Sorry, the alias is taken. Please choose another.");
		else if (!strcmp(res, "long\n"))
			strcpy(res_msg, "This alias is too long. Choose a shorter one.");
		else
			flag = 0;
		mvwaddstr(msg_win, 0, 0, res_msg);
		wrefresh(msg_win);
	}
	strcpy(res_msg, "The name has been assigned. Press a key to enter chat.");
	mvwaddstr(msg_win, 0, 0, res_msg);
	wrefresh(msg_win);
	getch();
	wclear(msg_win);
	wrefresh(msg_win);
}

void thread_send(void *sfd)
{
	int sockfd = *((int*)sfd);
	char buf[CON_LEN], msg[USR_MSG_LEN];
	wmove(ip_win, 0, 0);
	wrefresh(ip_win);
	while (1) {
		get_ip(buf, CON_LEN);
		if (!strcmp(buf, "quit()")) {
			flag = 0;
			exit(1);
		}
		pack_usr_msg(msg, buf, name);
		if (send(sockfd, msg, strlen(msg), 0) == -1) {
			perror("send");
			exit(1);
		}
		print_msg(name, buf);
		bzero(buf, CON_LEN);
		bzero(msg, USR_MSG_LEN);
		wmove(ip_win, 0, 0);
		wclear(ip_win);
		wrefresh(ip_win);
	}
}

void thread_recv(void *sfd)
{
	int sockfd = *((int*)sfd);
	char buf[USR_MSG_LEN], msg[USR_MSG_LEN];
	char sender_name[VAL_LEN];
	while (1) {
		if (recv(sockfd, msg, USR_MSG_LEN, 0) == -1) {
			perror("recv");
			exit(1);
		}
		unpack_usr_msg(msg, buf, sender_name);
		print_msg(sender_name, buf);
		bzero(buf, strlen(buf));
		bzero(msg, USR_MSG_LEN);
	}
}


int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "There must be 2 arguments (only)\n");
		exit(1);
	}

	init_ui();

	struct addrinfo *list = init_addrinfos(-1, AF_UNSPEC, SOCK_STREAM, argv[1], PORT);
	int sockfd;
	struct addrinfo *addr = bind_socket(list, &sockfd, 0);
	if (sockfd == -1)
		init_err_scr(sockfd, "Connection Failed", "No valid addrinfo struct", 1);
	else if (!addr)
		init_err_scr(1, "Connection Failed", "Socket could not connect to server", 1);

	login(sockfd);

	pthread_t t_send, t_recv;
	if (pthread_create(&t_send, NULL, (void *)thread_send, (void*)&sockfd)) {
		fprintf(stderr, "Cannot create the send thread.\n");
		return 1;
	}
	if (pthread_create(&t_recv, NULL, (void *)thread_recv, (void*)&sockfd)) {
		fprintf(stderr, "Cannot create the recv thread.\n");
		return 1;
	}
	while (1)
		if (!flag)
			break;

	close(sockfd);

	return 0;
}
