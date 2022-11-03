#include "message.h"

#include <string.h>

#define USER_MSG 1
#define CLIENT_MSG 0

#define SUB_MAX_LEN 10

void trim_str(char *str)
{
	for (int i = 0; i < strlen(str) && str[i] != '\0'; ++i)
		if (str[i] == '\n') {
			str[i] = '\0';
			break;
		}
}

enum client_msg_sub map_enum_str(char *msg, int *cur_len)
{
	char sub_str[SUB_MAX_LEN];
	*cur_len = 2; //1 for '!' + 1 for ':'
	int i;
	for (i = 1; msg[i] != ':'; ++i)
		sub_str[i - 1] = msg[i];
	sub_str[i - 1] = '\0';
	if (!strcmp(sub_str, "cname")) {
		*cur_len += 5;
		return CNAME;
	}
	*cur_len = -1;
	return FAILURE;
}

int pack_client_msg(char *msg, char *src, enum client_msg_sub sub)
{
	msg[0] = '!';
	switch (sub) {
		case CNAME:
			strcpy(msg + 1, "cname:");
			break;
		default:
			return -1;
	}

	int cur_len = strlen(msg);

	int i;
	for (i = 0; src[i] != '\0'; ++i)
		msg[cur_len + i] = src[i];
	msg[cur_len + i] = '\0';

	return 0;
}

int unpack_client_msg(char *msg, char *dst)
{
	int i, j;
	enum client_msg_sub sub = map_enum_str(msg, &i);

	for (j = 0; msg[i] != '\0'; ++j, ++i)
		dst[j] = msg[i];
	dst[j] = '\0';
	return 0;	
}

int pack_usr_msg(char *msg, char *src, char *name)
{
	msg[0] = '@';
	int i;
	for (i = 0; name[i] != '\0'; ++i)
		msg[i + 1] = name[i];
	msg[++i] = ':';
	for (int j = 0; src[j] != '\0'; ++j)
		msg[++i] = src[j];
	msg[++i] = '\0';

	return 0;
}

void unpack_usr_msg(char *msg, char *dst, char *name)
{
	int i;
	for (i = 0; msg[i + 1] != ':'; ++i)
		name[i] = msg[i + 1];
	name[i++] = '\0';

	int j;
	for (j = 0, i = i + 1; msg[i] != '\0'; ++j, ++i)
		dst[j] = msg[i];
	dst[j] = '\0';
}
