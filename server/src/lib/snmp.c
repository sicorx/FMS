/*
 * snmptrap.c - send snmp traps to a network entity.
 *
 */
/******************************************************************
    Copyright 1989, 1991, 1992 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/
#include "include.h"

oid     objid_enterprise[] = { 1, 3, 6, 1, 4, 1, 3, 1, 1 };
oid     objid_sysdescr[]   = { 1, 3, 6, 1, 2, 1, 1, 1, 0 };
oid     objid_sysuptime[]  = { 1, 3, 6, 1, 2, 1, 1, 3, 0 };
oid     objid_snmptrap[]   = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };
int     inform = 0;

extern struct equip_status *gp_status[];
extern struct equip_conn_info *conn_info[];

void initialize_snmp(void)
{
    init_snmp("snmpapp");
}

int initialize_oid(int module_idx)
{
#ifdef DEBUG
	fileLog(WARNING, "%s START!!!!\n", __FUNCTION__);
#endif
    int i;
    struct equip_status *pStatus = NULL;
    struct equip_conn_info *pConnInfo = NULL;

    pConnInfo = conn_info[module_idx];
    pStatus = gp_status[module_idx];

    for(i = 0; i < pConnInfo->ai_count; i++)
    {
        if(pStatus->_ai[i].dataOid.Name == NULL || !strncasecmp(pStatus->_ai[i].dataOid.Name, "UNKNOWN", 7) || pStatus->_ai[i].use_yn == NO)
        {
            continue;
        }
        pStatus->_ai[i].dataOid.OidLen = sizeof(pStatus->_ai[i].dataOid.Oid) / sizeof(pStatus->_ai[i].dataOid.Oid[0]);
        if(!read_objid( pStatus->_ai[i].dataOid.Name, pStatus->_ai[i].dataOid.Oid, (size_t *)&pStatus->_ai[i].dataOid.OidLen))
        {
            fileLog(INFO, "UPS eseq=[%d] AI read_objid error!! index(%d)\n", pConnInfo->eseq, i);
            return -1;
        }
    }

    for(i = 0; i < pConnInfo->di_count - 1; i++)
    {
        if(pStatus->_di[i].dataOid.Name == NULL || !strncasecmp(pStatus->_di[i].dataOid.Name, "UNKNOWN", 7) || pStatus->_di[i].use_yn == NO)
        {
            continue;
        }
        pStatus->_di[i].dataOid.OidLen = sizeof(pStatus->_di[i].dataOid.Oid) / sizeof(pStatus->_di[i].dataOid.Oid[0]);
        if(!read_objid( pStatus->_di[i].dataOid.Name, pStatus->_di[i].dataOid.Oid, (size_t *)&pStatus->_di[i].dataOid.OidLen))
        {
            fileLog(INFO, "UPS eseq=[%d] DI read_objid error!! index(%d)\n", pConnInfo->eseq, i);
            return -1;
        }
    }

    return 0;
}

int print_result_snmp(int status, struct snmp_session *sp, struct snmp_pdu *pdu, int module_idx, const char *type, int index)
{
	char oid_buf[256];
	char value_buf[1024];
	int  ix;
	struct variable_list *vp = NULL;
	
    struct equip_status *pStatus = NULL;
    struct equip_conn_info *pConnInfo = NULL;

    pConnInfo = conn_info[module_idx];
    pStatus = gp_status[module_idx];

	switch(status)
	{
		case STAT_SUCCESS :
			vp = pdu->variables;

			if(pdu->errstat == SNMP_ERR_NOERROR) 
			{
				while(vp)
				{
					snprint_objid(oid_buf, sizeof(oid_buf), vp->name, vp->name_length);
					if (vp->type == SNMP_NOSUCHOBJECT || vp->type == SNMP_NOSUCHINSTANCE || vp->type == SNMP_ENDOFMIBVIEW)
					{
						value_buf[0] = 0;
					}
					else
					{
						snprint_value(value_buf, sizeof(value_buf), vp->name, vp->name_length, vp);
					}
					vp = vp->next_variable;
				}
				set_module_status_snmp(oid_buf, value_buf, module_idx, type, index);
				return 0;
			}
			else
			{
				for(ix = 1; vp && ix != pdu->errindex; vp = vp->next_variable, ix++);

				if(vp) 
				{
					snprint_objid(oid_buf, sizeof(oid_buf), vp->name, vp->name_length);
				}
				else
				{
					strcpy(oid_buf, "(none)");
				}

				fileLog(INFO, "sp->peername[%s]: buf[%s]: snmp_errstring(pdu->errstat)[%s]\n", sp->peername, oid_buf, snmp_errstring(pdu->errstat));
				return -1;
			}


		case STAT_TIMEOUT :
			fileLog(CRITICAL, "UPS eseq=[%d] index(%02d) (%s, snmpget): Timeout\n", pConnInfo->eseq, index, sp->peername);
			return -1;

		case STAT_ERROR :
			snmp_perror(sp->peername);
			return -1;
	}

	return -1;
}

void set_module_status_snmp(const char *oid, char *value, int module_idx, const char *type, int index)
{
    int  send_flag=0, wbytes=0, prev_val=0, i;
    char buf[128]={0x00, }, trap_buff[512];
    char *ptr=NULL, *ptrptr=NULL, *endptr=NULL;
    struct equip_ai *pAI=gp_status[module_idx]->_ai+index;
    struct equip_di *pDI=gp_status[module_idx]->_di+index;
    struct equip_status *pStatus=gp_status[module_idx];
    struct equip_conn_info *pConnInfo=conn_info[module_idx];
	
	double ai_diff, di_diff, update_diff;
	time_t now;
	now = time(NULL);

    strcpy(buf, value);
    strtok_r(buf, ":", &ptrptr);

    ptr = strtok_r(NULL, " ", &ptrptr);
    if(ptr)
    {
        for(i=0; i<(int)strlen(ptr); i++)
            if(!isdigit(*(ptr+i)))
                *(ptr+i) = ' ';
    }
    else
	{
        ptr = "NULL";
	}

	if(strncmp(type, "AI", 2) == 0)
	{
		switch(pConnInfo->model_seq)
		{
			case SNMP_NETAGENT_MINI_1PHASE:
				if(index == 0)		pAI->curr_val = strtof(ptr, &endptr) / 10.;	//배터리 용량
				else if(index == 1)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//배터리 전압
				else if(index == 2)	pAI->curr_val = strtof(ptr, &endptr) / 10.; //배터리 온도
				else if(index == 3)	pAI->curr_val = strtof(ptr, &endptr) / 10.; //배터리 잔여 시간
				else if(index == 4)	pAI->curr_val = strtof(ptr, &endptr) / 10.; //배터리 충전률
				else if(index == 5)	pAI->curr_val = strtof(ptr, &endptr) / 10.; //배터리 가동시간
				else if(index == 6)	pAI->curr_val = strtof(ptr, &endptr);		//입력 전압
				else if(index == 7)	pAI->curr_val = strtof(ptr, &endptr);		//입력 주파수
				else if(index == 8)	pAI->curr_val = strtof(ptr, &endptr);		//출력 전압
				else if(index == 9)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//출력 주파수
				else if(index ==10)	pAI->curr_val = strtof(ptr, &endptr);		//출력 부하율
				else return;
				break;

			case SNMP_MGE_GALAXY_5000:
				if(index == 0)		pAI->curr_val = strtof(ptr, &endptr) / 10.;//배터리 전압
				else if(index == 1)	pAI->curr_val = strtof(ptr, &endptr);//배터리 전류
				else if(index == 2)	pAI->curr_val = strtof(ptr, &endptr);//배터리 온도
				else if(index == 3)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력주파수(R)
				else if(index == 4)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력주파수(S)
				else if(index == 5)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력주파수(T)
				else if(index == 6)	pAI->curr_val = strtof(ptr, &endptr);//입력 전압 R
				else if(index == 7)	pAI->curr_val = strtof(ptr, &endptr);//입력 전압 S
				else if(index == 8)	pAI->curr_val = strtof(ptr, &endptr);//입력 전압 T
				else if(index == 9)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력 전류 R
				else if(index ==10)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력 전류 S
				else if(index ==11)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력 전류 T
				else if(index ==12)	pAI->curr_val = strtof(ptr, &endptr);//출력상태(1/3)상
				else if(index ==13)	pAI->curr_val = strtof(ptr, &endptr) / 10;//출력 주파수
				else if(index ==14)	pAI->curr_val = strtof(ptr, &endptr);//출력 전압 R
				else if(index ==15)	pAI->curr_val = strtof(ptr, &endptr);//출력 전압 S
				else if(index ==16)	pAI->curr_val = strtof(ptr, &endptr);//출력 전압 T
				else if(index ==17)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//출력 전류 R
				else if(index ==18)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//출력 전류 S
				else if(index ==19)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//출력 전류 T
				else if(index ==20)	pAI->curr_val = strtof(ptr, &endptr) / 1000.;//출력 전력 R
				else if(index ==21)	pAI->curr_val = strtof(ptr, &endptr) / 1000.;//출력 전력 S
				else if(index ==22)	pAI->curr_val = strtof(ptr, &endptr) / 1000.;//출력 전력 T
				else if(index ==23)	pAI->curr_val = strtof(ptr, &endptr);//출력 로드율 R
				else if(index ==24)	pAI->curr_val = strtof(ptr, &endptr);//출력 로드율 S
				else if(index ==25)	pAI->curr_val = strtof(ptr, &endptr);//출력 로드율 T
				else if(index ==26)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//바이패스 주파수
				else if(index ==27)	pAI->curr_val = strtof(ptr, &endptr);//바이패스 전압 R
				else if(index ==28)	pAI->curr_val = strtof(ptr, &endptr);//바이패스 전압 S
				else if(index ==29)	pAI->curr_val = strtof(ptr, &endptr);//바이패스 전압 T
				else return;
				break;
				
			case SNMP_HIPULSE_U:
				if(index == 0)		pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전압 A
				else if(index == 1)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전압 B
				else if(index == 2)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전압 C
				else if(index == 3)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전류 A
				else if(index == 4)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전류 B
				else if(index == 5)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전류 C
				else if(index == 6)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 주파수
				else if(index == 7)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//바이패스 전압 A
				else if(index == 8)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//바이패스 전압 B
				else if(index == 9)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//바이패스 전압 C
				else if(index ==10)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//바이패스 주파수
				else if(index ==11)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 전압 A
				else if(index ==12)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 전압 B
				else if(index ==13)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 전압 C
				else if(index ==14)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 전류 A
				else if(index ==15)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 전류 B
				else if(index ==16)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 전류 C
				else if(index ==17)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 주파수
				else if(index ==18)	pAI->curr_val = strtof(ptr, &endptr);//출력 역률 A
				else if(index ==19)	pAI->curr_val = strtof(ptr, &endptr);//출력 역률 B
				else if(index ==20)	pAI->curr_val = strtof(ptr, &endptr);//출력 역률 C
				else if(index ==21)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 유효전력 A
				else if(index ==22)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 유효전력 B
				else if(index ==23)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 유효전력 C
				else if(index ==24)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 피상전력 A
				else if(index ==25)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 피상전력 B
				else if(index ==26)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 피상전력 C
				else if(index ==27)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 무효전력 A
				else if(index ==28)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 무효전력 B
				else if(index ==29)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 무효전력 C				
				else if(index ==30)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 로드율 A
				else if(index ==31)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 로드율 B
				else if(index ==32)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 로드율 C
				else if(index ==33)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//축전지 전압
				else if(index ==34)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//축전지 전류				
				else if(index ==35)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//병렬 시스템 유효전력 A
				else if(index ==36)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//병렬 시스템 유효전력 B
				else if(index ==37)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//병렬 시스템 유효전력 C
				else if(index ==38)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//병렬 시스템 피상전력 A
				else if(index ==39)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//병렬 시스템 피상전력 B
				else if(index ==40)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//병렬 시스템 피상전력 C
				else if(index ==41)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//병렬 시스템 무효전력 A
				else if(index ==42)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//병렬 시스템 무효전력 B
				else if(index ==43)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//병렬 시스템 무효전력 C
				else if(index ==44)	pAI->curr_val = strtof(ptr, &endptr);//시스템 이상
				else return;
				break;
				
			case SNMP_EMERSON_APM:
				if(index == 0)		pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전압 R
				else if(index == 1)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전압 S
				else if(index == 2)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전압 T
				else if(index == 3)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전압 RS
				else if(index == 4)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전압 ST
				else if(index == 5)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전압 TR
				else if(index == 6)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전류 R
				else if(index == 7)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전류 S
				else if(index == 8)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 전류 T
				else if(index == 9)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//입력 주파수
				else if(index ==10)	pAI->curr_val = strtof(ptr, &endptr);//입력 역률 R
				else if(index ==11)	pAI->curr_val = strtof(ptr, &endptr);//입력 역률 S
				else if(index ==12)	pAI->curr_val = strtof(ptr, &endptr);//입력 역률 T
				else if(index ==13)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//DC 버스 1 전압
				else if(index ==14)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//DC 버스 2 전압
				else if(index ==15)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 전압 R
				else if(index ==16)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 전압 S
				else if(index ==17)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 전압 T
				else if(index ==18)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 전류 R
				else if(index ==19)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 전류 S
				else if(index ==20)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 전류 T
				else if(index ==21)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 주파수
				else if(index ==22)	pAI->curr_val = strtof(ptr, &endptr);//출력 역률 R
				else if(index ==23)	pAI->curr_val = strtof(ptr, &endptr);//출력 역률 S
				else if(index ==24)	pAI->curr_val = strtof(ptr, &endptr);//출력 역률 T
				else if(index ==25)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 유효 전력 R
				else if(index ==26)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 유효 전력 S
				else if(index ==27)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 유효 전력 T
				else if(index ==28)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 무효 전력 R
				else if(index ==29)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 무효 전력 S			
				else if(index ==30)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 무효 전력 T
				else if(index ==31)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 피상 전력 R
				else if(index ==32)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 피상 전력 S
				else if(index ==33)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 피상 전력 T
				else if(index ==34)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 로드율 R		
				else if(index ==35)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 로드율 S
				else if(index ==36)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 로드율 T
				else if(index ==37)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 파고율 R
				else if(index ==38)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 파고율 S
				else if(index ==39)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//출력 파고율 T
				else if(index ==40)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//바이패스 전압 R
				else if(index ==41)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//바이패스 전압 S
				else if(index ==42)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//바이패스 전압 T
				else if(index ==43)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//바이패스 주파수
				else if(index ==44)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//축전지 전압 (+)
				else if(index ==45)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//축전지 전류 (+)
				else if(index ==46)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//축전지 전압 (-)
				else if(index ==47)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//축전지 전류 (-)
				else if(index ==48)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//축전지 Aging Rate
				else if(index ==49)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//축전지 남은 시간
				else if(index ==50)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//축전지 온도
				else if(index ==51)	pAI->curr_val = strtof(ptr, &endptr) / 100.;//축전지 룸 온도
				else if(index ==52)	pAI->curr_val = strtof(ptr, &endptr);//써머리 경보
				else return;
				break;
				
			case SNMP_HIPULSE_V1310:
				if(index == 0)		pAI->curr_val = strtof(ptr, &endptr) / 10.;//배터리 전압
				else if(index == 1)	pAI->curr_val = strtof(ptr, &endptr);//배터리 전류
				else if(index == 2)	pAI->curr_val = strtof(ptr, &endptr);//배터리 온도
				else if(index == 3)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력주파수(R)
				else if(index == 4)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력주파수(S)
				else if(index == 5)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력주파수(T)
				else if(index == 6)	pAI->curr_val = strtof(ptr, &endptr);//입력 전압 R
				else if(index == 7)	pAI->curr_val = strtof(ptr, &endptr);//입력 전압 S
				else if(index == 8)	pAI->curr_val = strtof(ptr, &endptr);//입력 전압 T
				else if(index == 9)	pAI->curr_val = strtof(ptr, &endptr);//출력상태(1/3)상
				else if(index ==10)	pAI->curr_val = strtof(ptr, &endptr) / 10;//출력 주파수
				else if(index ==11)	pAI->curr_val = strtof(ptr, &endptr);//출력 전압 R
				else if(index ==12)	pAI->curr_val = strtof(ptr, &endptr);//출력 전압 S
				else if(index ==13)	pAI->curr_val = strtof(ptr, &endptr);//출력 전압 T
				else if(index ==14)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//출력 전류 R
				else if(index ==15)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//출력 전류 S
				else if(index ==16)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//출력 전류 T
				else if(index ==17)	pAI->curr_val = strtof(ptr, &endptr) / 1000.;//출력 전력 R
				else if(index ==18)	pAI->curr_val = strtof(ptr, &endptr) / 1000.;//출력 전력 S
				else if(index ==19)	pAI->curr_val = strtof(ptr, &endptr) / 1000.;//출력 전력 T
				else if(index ==20)	pAI->curr_val = strtof(ptr, &endptr);//출력 로드율 R
				else if(index ==21)	pAI->curr_val = strtof(ptr, &endptr);//출력 로드율 S
				else if(index ==22)	pAI->curr_val = strtof(ptr, &endptr);//출력 로드율 T
				else if(index ==23)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//바이패스 주파수
				else if(index ==24)	pAI->curr_val = strtof(ptr, &endptr);//바이패스 전압 R
				else if(index ==25)	pAI->curr_val = strtof(ptr, &endptr);//바이패스 전압 S
				else if(index ==26)	pAI->curr_val = strtof(ptr, &endptr);//바이패스 전압 T
				else return;
				break;
				
			case SNMP_TAEIL_MARCHE:
				if(index == 0)		pAI->curr_val = strtof(ptr, &endptr) / 10.;//배터리 전압
				else if(index == 1)	pAI->curr_val = strtof(ptr, &endptr);//배터리 전류
				else if(index == 2)	pAI->curr_val = strtof(ptr, &endptr);//배터리 온도
				else if(index == 3)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력주파수(R)
				else if(index == 4)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력주파수(S)
				else if(index == 5)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력주파수(T)
				else if(index == 6)	pAI->curr_val = strtof(ptr, &endptr);//입력 전압 R
				else if(index == 7)	pAI->curr_val = strtof(ptr, &endptr);//입력 전압 S
				else if(index == 8)	pAI->curr_val = strtof(ptr, &endptr);//입력 전압 T
				else if(index == 9)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력 전류 R
				else if(index ==10)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력 전류 S
				else if(index ==11)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//입력 전류 T
				else if(index ==12)	pAI->curr_val = strtof(ptr, &endptr);//출력상태(1/3)상
				else if(index ==13)	pAI->curr_val = strtof(ptr, &endptr) / 10;//출력 주파수
				else if(index ==14)	pAI->curr_val = strtof(ptr, &endptr);//출력 전압 R
				else if(index ==15)	pAI->curr_val = strtof(ptr, &endptr);//출력 전압 S
				else if(index ==16)	pAI->curr_val = strtof(ptr, &endptr);//출력 전압 T
				else if(index ==17)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//출력 전류 R
				else if(index ==18)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//출력 전류 S
				else if(index ==19)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//출력 전류 T
				else if(index ==20)	pAI->curr_val = strtof(ptr, &endptr) / 1000.;//출력 전력 R
				else if(index ==21)	pAI->curr_val = strtof(ptr, &endptr) / 1000.;//출력 전력 S
				else if(index ==22)	pAI->curr_val = strtof(ptr, &endptr) / 1000.;//출력 전력 T
				else if(index ==23)	pAI->curr_val = strtof(ptr, &endptr);//출력 로드율 R
				else if(index ==24)	pAI->curr_val = strtof(ptr, &endptr);//출력 로드율 S
				else if(index ==25)	pAI->curr_val = strtof(ptr, &endptr);//출력 로드율 T
				else if(index ==26)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//바이패스 주파수
				else if(index ==27)	pAI->curr_val = strtof(ptr, &endptr);//바이패스 전압 R
				else if(index ==28)	pAI->curr_val = strtof(ptr, &endptr);//바이패스 전압 S
				else if(index ==29)	pAI->curr_val = strtof(ptr, &endptr);//바이패스 전압 T
				else if(index ==30)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//바이패스 전류 R
				else if(index ==31)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//바이패스 전류 S
				else if(index ==32)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//바이패스 전류 T
				else if(index ==33)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//바이패스 전력 R
				else if(index ==34)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//바이패스 전력 S
				else if(index ==35)	pAI->curr_val = strtof(ptr, &endptr) / 10.;//바이패스 전력 T
				else return;
				break;
				
			// STS 시작
			case MGE:
				if(index == 0)		pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S1-전원전압(RS)
				else if(index == 1)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S1-전원전압(ST)
				else if(index == 2)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S1-전원전압(TR)
				else if(index == 3)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S1-전원전류(R)
				else if(index == 4)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S1-전원전류(S)
				else if(index == 5)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S1-전원전류(T)
				else if(index == 6)	pAI->curr_val = strtof(ptr, &endptr);	//S1-전원피상전력(R)
				else if(index == 7)	pAI->curr_val = strtof(ptr, &endptr);	//S1-전원피상전력(S)
				else if(index == 8)	pAI->curr_val = strtof(ptr, &endptr);	//S1-전원피상전력(T)
				else if(index == 9)	pAI->curr_val = strtof(ptr, &endptr);	//S1-전원유효전력(R)
				else if(index ==10)	pAI->curr_val = strtof(ptr, &endptr);	//S1-전원유효전력(S)
				else if(index ==11)	pAI->curr_val = strtof(ptr, &endptr);	//S1-전원유효전력(T)
				else if(index ==12)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S1-N상전압(R)
				else if(index ==13)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S1-N상전압(S)
				else if(index ==14)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S1-N상전압(T)
				else if(index ==15)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S1-N상전류
				else if(index ==16)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S1-주파수

				else if(index ==17)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S2-전원전압(RS)   
				else if(index ==18)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S2-전원전압(ST)   
				else if(index ==19)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S2-전원전압(TR)   
				else if(index ==20)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S2-전원전류(R)    
				else if(index ==21)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S2-전원전류(S)    
				else if(index ==22)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S2-전원전류(T)    
				else if(index ==23)	pAI->curr_val = strtof(ptr, &endptr);	//S2-전원피상전력(R) 
				else if(index ==24)	pAI->curr_val = strtof(ptr, &endptr);	//S2-전원피상전력(S) 
				else if(index ==25)	pAI->curr_val = strtof(ptr, &endptr);	//S2-전원피상전력(T) 
				else if(index ==26)	pAI->curr_val = strtof(ptr, &endptr);	//S2-전원유효전력(R) 
				else if(index ==27)	pAI->curr_val = strtof(ptr, &endptr);	//S2-전원유효전력(S) 
				else if(index ==28)	pAI->curr_val = strtof(ptr, &endptr);	//S2-전원유효전력(T) 
				else if(index ==29)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S2-N상전압(R)    
				else if(index ==30)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S2-N상전압(S)    
				else if(index ==31)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S2-N상전압(T)    
				else if(index ==32)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S2-N상전류       
				else if(index ==33)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//S2-주파수        
	
				else if(index ==34)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//출력N상전압(R)   
				else if(index ==35)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//출력N상전압(S)
				else if(index ==36)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//출력N상전압(T)
				else if(index ==37)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//출력전압(R)
				else if(index ==38)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//출력전압(S)
				else if(index ==39)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//출력전압(T)
				else if(index ==40)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//출력전류(R)
				else if(index ==41)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//출력전류(S)
				else if(index ==42)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//출력전류(T)
				else if(index ==43)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//출력N상전류
				else if(index ==44)	pAI->curr_val = strtof(ptr, &endptr) / 10.;	//출력주파수
				else return;
				break;
		}
		
		if(pAI->use_yn == YES)
		{
			if(pAI->alarm_yn == YES)
			{
				if(pAI->tm_alarm==0) pAI->tm_alarm = time(NULL);

				if((pAI->curr_val > pAI->critical_high || pAI->curr_val < pAI->critical_low) && pAI->critical_alarm_yn == YES)
					pAI->curr_status = CRITICAL;
				else if((pAI->curr_val > pAI->warning_high || pAI->curr_val < pAI->warning_low) && pAI->warning_alarm_yn == YES)
					pAI->curr_status = WARNING;
				else if((pAI->curr_val > pAI->info_high || pAI->curr_val < pAI->info_low) && pAI->info_alarm_yn == YES)
					pAI->curr_status = INFO;
				else
				{
					pAI->curr_status = NORMAL;
					pAI->tm_alarm = time(NULL);
				}
				
				// 알람 일때
				if(pAI->send_flag==0 && pAI->curr_status != NORMAL )
				{
					ai_diff = difftime(now, pAI->tm_alarm);
					if(ai_diff >= pAI->alarm_term)
					{
						pAI->send_flag = 1;
						alarm_insert(OCCURE, index, AI, i, pAI->curr_status);
						pAI->last_status = pAI->curr_status;
						//LOG_ALARM INSERT
					}
				}
				// 더 상위 알람
				else if(pAI->send_flag==1 && pAI->last_status < pAI->curr_status)
				{
					alarm_insert(CLEAR, index, AI, i, pAI->last_status);
					alarm_insert(OCCURE, index, AI, i, pAI->curr_status);
					pAI->last_status = pAI->curr_status;
				}
				// 알람이 복구 될때
				else if(pAI->send_flag == 1 && pAI->curr_status == NORMAL)
				{
					pAI->send_flag = 0;
					alarm_insert(CLEAR, index, AI, i, pAI->last_status);
					//LOG_ALARM RECOVER
				}
			}
		}


		ai_diff = difftime(now, pAI->tm_save);
		if(pAI->save_yn==YES && (ai_diff >= pAI->save_term || pAI->tm_save == 0) )
		{
			mysqlinsertData(pConnInfo->eseq, pAI->tseq, pAI->curr_val);
			pAI->tm_save = time(NULL);
		}

		/* update가 느려서 임시 처리
		if(update_diff >= pConnInfo->update_term || pConnInfo->tm_update == 0)
		{
			mysqlUpdateTag(pConnInfo->eseq, pAI->tseq, pAI->curr_val);
			pConnInfo->tm_update = time(NULL);
		}
		*/
		mysqlUpdateTag(pConnInfo->eseq, pAI->tseq, pAI->curr_val);
		pConnInfo->tm_update = time(NULL);
	}
	else if(strncmp(type, "DI", 2) == 0)
	{
		switch(pConnInfo->model_seq)
		{
			case SNMP_NETAGENT_MINI_1PHASE :
				if(index == 0)		pDI->curr_val = (atoi(ptr)==2) ? DI_ON : DI_OFF;	//on Battery
				else if(index == 1) pDI->curr_val = (atoi(ptr)==3) ? DI_ON : DI_OFF;	//on Bypass
				else return;
				break;
		}

		if(pDI->use_yn == YES)
		{
			if(pDI->alarm_yn == YES)
			{
				if(pDI->tm_alarm==0) pDI->tm_alarm = time(NULL);

				if(pDI->curr_val == pDI->alarm_val && pDI->critical_alarm_yn == YES)
					pDI->curr_status = CRITICAL;
				else
				{
					pDI->curr_status = NORMAL;
					pDI->tm_alarm = time(NULL);
				}
				
				// 알람 일때
				if(pDI->send_flag==0 && pDI->curr_status != NORMAL )
				{
					di_diff = difftime(now, pDI->tm_alarm);
					if(di_diff >= pDI->alarm_term)
					{
						pDI->send_flag = 1;
						alarm_insert(OCCURE, index, DI, i, pDI->curr_status);
						pDI->last_status = pDI->curr_status;
					}
				}
				// 알람이 복구 될때
				else if(pDI->send_flag == 1 && pDI->curr_status == NORMAL)
				{
					pDI->send_flag = 0;
					alarm_insert(CLEAR, index, DI, i, pDI->last_status);
				}
			}
		}


		di_diff = difftime(now, pDI->tm_save);
		if(pDI->save_yn==YES && (di_diff >= pDI->save_term || pDI->tm_save == 0))
		{
			mysqlinsertData(pConnInfo->eseq, pDI->tseq, pDI->curr_val);
			pDI->tm_save = time(NULL);
		}
		update_diff = difftime(now, pConnInfo->tm_update);

		/* 업데이가 느려서 임시처리
		if(update_diff >= pConnInfo->update_term || pConnInfo->tm_update == 0)
		{
			mysqlUpdateTag(pConnInfo->eseq, pDI->tseq, pDI->curr_val);
			pConnInfo->tm_update = time(NULL);
		}
		*/
		mysqlUpdateTag(pConnInfo->eseq, pDI->tseq, pDI->curr_val);
		pConnInfo->tm_update = time(NULL);
	}

#ifdef DEBUG
    fileLog(INFO, "SNMP(%d) %s[%02d] oid(%s) name[%s] value(%s)\n", module_idx+1, (strcmp(type, "AI") == 0) ? "AI" : "DI", index, oid, (strcmp(type, "AI") == 0) ? pAI->tag_nm : pDI->tag_nm, value);
#endif
}

int get_synchronous_snmp(int equip_idx)
{
	int status, rvalue, idx;
	struct equip_status *pStatus = NULL;
	struct equip_conn_info *pConnInfo = NULL;
	struct snmp_session session, *sp = NULL;
	struct snmp_pdu *req = NULL, *resp = NULL;

#ifdef DEBUG
	fileLog(WARNING, "UPS(%d) get synchronous snmp start..\n", equip_idx+1);
#endif
	
	pConnInfo = conn_info[equip_idx];
	pStatus = gp_status[equip_idx];

	netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_DEFAULT_PORT, SNMP_PORT);
	netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT, NETSNMP_OID_OUTPUT_FULL);
	netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT, NETSNMP_OID_OUTPUT_NUMERIC);

	snmp_sess_init(&session);

	session.version  = SNMP_VERSION_1;
	
	session.peername       = pConnInfo->ip;
	session.community      = (u_char *)pConnInfo->snmp_read_community;
	session.community_len  = strlen((const char *)session.community);
	session.remote_port    = SNMP_PORT;

	SOCK_STARTUP;
	
	if(!(sp = snmp_open(&session)))
	{
		fileLog(CRITICAL, "UPS eseq=[%d] snmp open error!!\n", pConnInfo->eseq);
		return -1;
	}

    for(rvalue = idx = 0; idx < pConnInfo->ai_count; idx++)
    {
		if(pStatus->_ai[idx].use_yn == NO)
		{
			continue;
		}

        req = snmp_pdu_create(SNMP_MSG_GET);

        snmp_add_null_var(req, pStatus->_ai[idx].dataOid.Oid, pStatus->_ai[idx].dataOid.OidLen);
        status = snmp_synch_response(sp, req, &resp);

        if(0 != print_result_snmp(status, sp, resp, equip_idx, "AI", idx)) rvalue++;

        snmp_free_pdu(resp);
        if(rvalue)
        {
            fileLog(CRITICAL, "UPS eseq=[%d] AI(%02d) get error.. AI snmpget SKIP !!\n", pConnInfo->eseq, idx+1);
            break;
        }
    }

	for(rvalue = idx = 0; idx < pConnInfo->di_count - 1; idx++)
	{
		if(pStatus->_di[idx].use_yn == NO)
		{
			continue;
		}

		req = snmp_pdu_create(SNMP_MSG_GET);
		snmp_add_null_var(req, pStatus->_di[idx].dataOid.Oid, pStatus->_di[idx].dataOid.OidLen);
		status = snmp_synch_response(sp, req, &resp);

		if(0 != print_result_snmp(status, sp, resp, equip_idx, "DI", idx)) rvalue++;

		snmp_free_pdu(resp);
		if(rvalue)
		{
			fileLog(CRITICAL, "UPS eseq=[%d] DI(%02d) get error.. DI snmpget SKIP !!\n", pConnInfo->eseq, idx+1);
			break;
		}
	}


	snmp_close(sp);

	SOCK_CLEANUP;

#ifdef DEBUG
	fileLog(WARNING, "UPS eseq=[%d] get synchronous snmp finish..\n", pConnInfo->eseq);
#endif

    return rvalue;
}
