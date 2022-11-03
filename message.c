#include "message.h"

#include <string.h>

#define USER_MSG 1
#define CLIENT_MSG 0

enum client_msg_sub map_enum_str(char *msg, int msg_len, int *cur_len)
{
	char sub_str[SUB_MAX_LEN];
	*cur_len = 2; //1 for '!' + 1 for ':'
	for (int i = 1; i < msg_len && msg[i] != ':'; ++i)
		sub_str[i - 1] == msg[i];
	if (!strcmp(sub_str, "cname")) {
		*cur_len += 5;
		return CNAME;
	}
	*cur_len = -1;
	return FAILURE;
}

int pack_client_msg(char *msg, int msg_len, char *src, int src_len, enum client_msg_sub sub)
{
	if (msg_len < src_len)
		return -1;

	msg[0] = '!';
	switch (sub) {
		case CNAME:
			strcpy(msg + 1, "cname:");
			break;
		case default:
			return -1;
	}

	int cur_len = strlen(msg);
	if (cur_len + src_len >= msg_len)
		return -1;

	for (int i = 0; i < src_len; ++i) {
		if (src[i] == '\n')
			msg[cur_len + i] = '\0';
		else
			msg[cur_len + i] = src[i];
		if (msg[cur_len + i] == '\0')
			break;
	}
	return 0;
}

int unpack_client_msg(char *msg, int msg_len, char *dst, int dst_len)
{
	int i, j;
	enum client_msg_sub sub = map_enum_str(msg, msg_len, &i);
	for (j = 0; i < msg_len && j < dst_len; ++i, ++j) {
		if (msg[i] == '\n')
			dst[j] = '\0';
		else
			dst[j] = msg[i];
		if (dst[j] == '\0')
			break;
	}

	if (dst[j] != '\0')
		dst[j] = '\0';
	return 0;	
}
