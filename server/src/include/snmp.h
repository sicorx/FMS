#ifndef _SNMP_H_
#define _SNMP_H_ 1

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

void initialize_snmp(void);
int initialize_oid(int module_idx);
void set_module_status_snmp_abnormal(int module_idx, int com_state);
int get_synchronous_snmp(int equip_idx);
int print_result_snmp(int status, struct snmp_session *sp, struct snmp_pdu *pdu, int module_idx, const char *type, int index);
void set_data_buff(const char *oid, char *value, int module_no, int index);
int snmp_input(int operation, netsnmp_session * session, int reqid, netsnmp_pdu *pdu, void *magic);
void set_module_status_snmp(const char *oid, char *value, int module_idx, const char *type, int index);
#endif /* _SNMP_H_ */
