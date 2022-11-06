#ifndef UI
#define UI

void init_ui(); /* initializes ui as a whole */
void init_root_ui(); /* initializes the terminal */
void init_msg_win(); /* window showing all messages */
void init_ip_win(); /* window where user types message */
void init_err_scr(int err, char *title, char *subtitle, int should_exit);
void print_msg(char *name, char *buf);
void get_ip(char *buf, int len);

#endif
