
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
#define LOG_FILE	LOG_DIR"/SMS.log"

#define CONF_DIR	"/home/fms/server/conf"
#define CONF_FILE	CONF_DIR"/sms.conf"

#define TERMINAL_COLOR_RESET	"\x1b[0m"
#define TERMINAL_COLOR_RED		"\x1b[31m"
#define TERMINAL_COLOR_YELLOW	"\x1b[33m"

#define ALARM		-1
#define NORMAL		0
#define INFO		1
#define WARNING		2
#define CRITICAL	3

#define MAX_ALARM_LIMIT_COUNT		1
#define MAX_RECEIVER_LIMIT_COUNT	1024

#define SMS_SERVER_IP	"172.30.10.203"
#define SMS_PORT		53043

//#define SMS_SERVER_IP	"192.168.0.101"
//#define SMS_PORT		53043
#define SMS_LO			"O"
#define RESERVED_TIME	"00"
#define SEND_NUMBER		"01022330192" //UNUSED
#define BUF_LEN			1024

struct alarm_list 
{
	unsigned int	aseq;	//alarm_seq
	unsigned int	eseq;	//equip_seq
	unsigned int	tseq;	//tag_seq
	unsigned char	alarm_grade_cd;
	double			curr_value;
	double			th_value_low;
	double			th_value_high;
	char			sms_yn[4];
	char			sms_send_yn[4];
	char			sms_send_r_yn[4];
	unsigned char	alarm_status;
	char			nv_nm[32];
	char			equip_nm[64];
	char			tag_nm[64];
	char			normal_str[32];
	char			alarm_str[32];
};

struct receiver_list
{
	char user_id[32];
	char user_nm[32];
	char user_tel[32];
};

struct equip
{
	//select 후 결과
};

struct tag
{
	//select 후 결과
};



void print_start_log(void);
void print_end_log(void);
void fileLog(int flag, const char *fmt, ... );
void err_exit(int flag, const char *fmt, ... );
void space_rtrim(char *value);
int get_key_value(const char *filename, const char *key, char *value);
void get_conf(void);
int mysql_con_init(void);

int get_alarm_list(void);
void list_init(void);
int get_receiver_list(int idx);