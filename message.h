/* message -- packs and unpacks strings to be sent into messages */

#ifndef MESSAGE
#define MESSAGE

/* MESSAGES:
 * All messages mandatorily must end with '\0'
 * There are 2 types of messages:
 *	1. client messages: sent by the client with a specific request,
 *	such as alias lookup
 *	2. user messages: sent by the user to other users
 *
 * 1. CLIENT MESSAGES:
 * they begin with an exclamation mark '!'
 * syntax: !sub:val
 * "sub" refers to the command (such as "cname", which asks the server to look
 * up the alias)
 * "val" refers to the value associated with the command (such as the name for
 * "cname")
 * ':' is always necessary for separating the sub and val.
 *
 * 2. USER MESSAGES:
 * they begin with '@'
 * syntax: @name:message
 * "name" refers to the alias of the user who sent the message
 * "message" is the actual text that the user sends
 * ':' is always necessary
 */

enum client_msg_sub {
	CNAME = 0,
	FAILURE
};

void trim_str(char *str);
enum client_msg_sub map_enum_str(char *msg, int *cur_len);
int pack_client_msg(char *msg, char *src, enum client_msg_sub sub);
int unpack_client_msg(char *msg, char *dst);
int pack_usr_msg(char *msg, char *src, char *name);
void unpack_usr_msg(char *msg, char *dst, char *name);

#endif
