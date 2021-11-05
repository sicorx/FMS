/***************************************************************
Made By Hong-SeungHyeon
Date: 2020-10-20(ȭ)
***************************************************************/

#ifndef _INCLUDE_H_
#define _INCLUDE_H_ 1

//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
#include <arpa/inet.h>

//ccccccccccccccccccccccccccccccccccc
#include <ctype.h>

//ddddddddddddddddddddddddddddddddddd
#include <dirent.h>

//eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
#include <errno.h>

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




#include "common.h"
#include "crclib.h"
#include "db.h"
#include "dbset.h"
#include "define.h"
#include "log.h"
#include "modbus.h"
#include "model.h"
#include "ping.h"
#include "protocol_recv.h"
#include "protocol_send.h"
#include "read.h"
#include "struct.h"
#include "snmp.h"
#include "write.h"

#include "def_air.h"
#include "def_bms.h"
#include "def_dpm.h"
#include "def_ec.h"
#include "def_er.h"
#include "def_leak.h"
#include "def_th.h"
#include "def_ups.h"
#include "def_sts.h"

#endif /* _INCLUDE_H_ */
