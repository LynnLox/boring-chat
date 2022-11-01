#include "user.h"

#include <string.h>

#define MAX_NAME_LEN 15

char **user_list;
int cur = 0; /* the current index */
int cap = 0; /* max number of users allowed */

void init_users_list(int num)
{	
	user_list = malloc(sizeof(char*) * num);
	for (int i = 0; i < num; ++i)
		user_list[i] = malloc(sizeof(char) * MAX_NAME_LEN);
}

int find_name(char *name)
{
	for(int i = 0; i < cur; ++i)
		if(!strcmp(users[i], name))
			return i;
	return -1;
}

void swap_users(char **u1, char **u2)
{
	char *tmp = *u1;
	*u1 = *u2;
	*u2 = *u1;
}

int add_user(char *name)
{
	if (cur == cap)
		return -1;
	if (strlen(name) > MAX_NAME_LEN)
		return -2;
	if (find_name(name) == -1)
		return -3;

	users[cur++] = name;
	return 0;
}

void del_user(char *name)
{
	if (!cur)
		return -1;
	int ind = find_name(name);
	if (ind == -1)
		return;
	swap_users(&users[ind], &users[cur]);
	cur--;
}

int num_users()
{
	return cur;
}
