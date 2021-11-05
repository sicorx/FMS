
//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
#include <arpa/inet.h>

//ccccccccccccccccccccccccccccccccccc
#include <ctype.h>

//ddddddddddddddddddddddddddddddddddd
#include <dirent.h>

//eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
#include <errno.h>

//iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
#include <iconv.h>
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
#include <mysql.h>

//nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn
#include <math.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
//ppppppppppppppppppppppppppppppppppp
#include <pthread.h>
#include <pwd.h>

//sssssssssssssssssssssssssssssssssss
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>


//ttttttttttttttttttttttttttttttttttt
#include <time.h>

//uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu
#include <unistd.h>

#define LOG_DIR		"/home/fms/server/log"
#define LOG_FILE	LOG_DIR"/TRAP.log"

#define CONF_DIR	"/home/fms/server/conf"
#define CONF_FILE	CONF_DIR"/trap.conf"

#define TERMINAL_COLOR_RESET	"\x1b[0m"
#define TERMINAL_COLOR_RED		"\x1b[31m"
#define TERMINAL_COLOR_YELLOW	"\x1b[33m"

#define ALARM		-1
#define NORMAL		0
#define INFO		1
#define WARNING		2
#define CRITICAL	3
#define DATA		4

#define MAX_ALARM_LIMIT_COUNT		1
#define MAX_RECEIVER_LIMIT_COUNT	1024

#define VERSION1	0
#define VERSION2	1

#define MAXLINE    511
#define SNMP_SOCK_PORT 162

void print_start_log(void);
void print_end_log(void);
void fileLog(int flag, const char *fmt, ... );
void err_exit(int flag, const char *fmt, ... );
void space_rtrim(char *value);
int get_key_value(const char *filename, const char *key, char *value);
void get_conf(void);
int mysql_con_init(void);
