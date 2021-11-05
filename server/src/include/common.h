/***************************************************************
Made By Hong-SeungHyeon
Date: 2020-10-20(화)
***************************************************************/

#ifndef _COMMON_H_
#define _COMMON_H_ 1

void handler(int signo);
void sig_chld(int signo);
void init_signal(void);
int set_linger(int socket_fd, int on_off, int linger_timeout);
int makeServer(unsigned short portnum);
char get_hex(signed char value);
int connectServer(char *remoteHost, unsigned short portnum);
void space_rtrim(char *value);
int get_key_value(const char *filename, const char *key, char *value);
int getid(void);

#endif /* _COMMON_H_ */
