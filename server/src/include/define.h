#ifndef _DEFINE_H_
#define _DEFINE_H_ 1

#define DB_SERVER	"127.0.0.1"
#define DB_USER		"kis"
#define DB_PASSWD	"kis1200"
#define DB_DATABASE	"FMS"

#define LOG_DIR		"/home/fms/server/log"
#define LOG_FILE	LOG_DIR"/OBSERVER.log"

#define CONF_DIR	"/home/fms/server/conf"
#define CONF_FILE	CONF_DIR"/observer.conf"

#define TERMINAL_COLOR_RESET	"\x1b[0m"
#define TERMINAL_COLOR_RED		"\x1b[31m"
#define TERMINAL_COLOR_YELLOW	"\x1b[33m"

#define MAX_SERVICE_COUNT	400
#define EQUIP_SEQ		432

#define YES			1
#define NO			-1

#define ALARM		-1
#define NORMAL		0
#define INFO		1
#define WARNING		2
#define CRITICAL	3

#define STX     0x02    /* START    */
#define ETX     0x03    /* END      */

#define DI_ON   0x01    /* DI ON    */
#define DI_OFF  0x00    /* DI OFF   */
#define DO_ON   0x01    /* DO ON    */
#define DO_OFF  0x00    /* DO OFF   */

#define TYPE_INT	0x01
#define TYPE_FLOAT	0x02
#define TYPE_DOUBLE	0x03

#define TIMEOUT                 0x03

#define LISTEN_QUEUE_SIZE       100

#define COM_NORMAL      0x01
#define COM_ABNORMAL    0x00

#define TRAP_MAX_LEN            256     /* 128x2 */

#define RECV			1
#define SEND			2

#define AI				1
#define AO				2
#define DI				3
#define DO				4

#define OCCURE			1
#define CLEAR			2

#define MODBUS_UNDEFINED_REQ_SIZE   -1

#define READ_COILS				1
#define READ_DISCREATE_INPUTS	2
#define READ_HOLDING_REGISTERS	3
#define READ_INPUT_REGISTERS	4
#define WRITE_SINGLE_COIL		5
#define WRITE_SINGLE_REGISTER	6

#define DEV_UNKNOWN_TYPE        0x00    /* 알수없는 디바이스 타입 */
#define DEV_ETHERNET_TYPE       0x01    /* Ethernet 디바이스 타입 */
#define DEV_SERIAL_TYPE         0x02    /* 시리얼 디바이스 타입 */
#define DEV_SNMP_TYPE           0x03    /* SNMP 디바이스 타입 */

#define STR_DEV_UNKNOWN_TYPE	"UNKNOWN"
#define STR_DEV_ETHERNET_TYPE	"Ethernet"
#define STR_DEV_SERIAL_TYPE		"SERIAL"
#define STR_DEV_SNMP_TYPE		"SNMP"

#define HEADER_UNKNOWN_TYPE		0x00
#define HEADER_MODBUS_TCP		0x01

#define STR_HEADER_UNKNOWN_TYPE	"UNKNOWN"
#define STR_HEADER_MODBUS_TCP	"MODBUS_TCP"

#define TAIL_UNKNOWN_TYPE	0x00
#define TAIL_MODBUS_CRC16	0x01

#define STR_TAIL_UNKNOWN_TYPE	"UNKNOWN"
#define STR_TAIL_MODBUS_CRC16	"CRC16"

#define MODBUS_SERVER_PORT		1502
#define THRESHOLD_SERVER_PORT	6020
#endif /* _DEFINE_H_ */
