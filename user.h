/* user -- deals with information about all connected users */

#ifndef USER
#define USER

void init_users_list(int num);
int find_name(char *name);
void swap_users(char **u1, char **u2);
int add_user(char *name);
void del_user(char *name);
int num_users();

#endif
